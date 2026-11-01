#include "Render/AnimationPlayer.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <utility>
#include <vector>

#include <raymath.h>

#include "Render/Pool.hpp"
#include "BackendAccess.hpp"

namespace Lil::Render {

    namespace {

        // One-time (per process, per reason) warnings so a player that
        // silently refuses to animate can be diagnosed from the log.
        enum class WarnReason : uint32_t {
            NotLoadedOrStale,
            LoadNoModel,
            LoadNoAnimations,
            LoadNoBones,
            PlayRejected,
            NeverPlayed,
            ZeroDelta,
            MissingMeshData,
            BadAnimation,
            CloneFailed,
        };

        static uint32_t s_warned = 0;

        static bool ShouldWarn(WarnReason reason) {
            const uint32_t bit = 1u << static_cast<uint32_t>(reason);

            if (s_warned & bit) {
                return false;
            }

            s_warned |= bit;
            return true;
        }

        struct PlayerSlot {
            Assets::Model model;
            Assets::AnimationLib library;

            uint32_t generation = 0;

            int animationIndex = -1;
            float elapsedTime = 0.0f;
            bool playing = false;

            std::vector<bool> loop;
        };

        static Pool<PlayerSlot> s_players;

        // A slot only stores asset *handles* (keys), never raw pointers, and
        // every operation re-resolves them. So when the asset generation
        // changes (some model/lib was replaced or removed) the slot does not
        // have to die: if its model and library still exist, re-sync it and
        // carry on. Without this, removing or replacing ANY asset left every
        // player permanently frozen (Update/Play/Draw all became no-ops).
        static bool Revalidate(PlayerSlot& slot) {
            ::Model* model = Backend::GetRaylibModel(slot.model);

            unsigned int animationCount = 0;
            ModelAnimation* animations =
                Backend::RaylibAnimationLib(
                    slot.library,
                    &animationCount
                );

            if (!model ||
                !animations ||
                animationCount == 0 ||
                model->boneCount <= 0) {
                return false;
            }

            // Keeps existing loop flags; new animations default to looping.
            slot.loop.resize(animationCount, true);

            const int index = slot.animationIndex;

            if (index >= 0 &&
                (index >= static_cast<int>(animationCount) ||
                 animations[index].frameCount <= 0 ||
                 !IsModelAnimationValid(*model, animations[index]))) {
                slot.animationIndex = -1;
                slot.elapsedTime = 0.0f;
                slot.playing = false;
            }

            slot.generation = Backend::AssetGeneration();
            return true;
        }

        static PlayerSlot* Live(uint32_t id) {
            PlayerSlot* slot = s_players.Get(id);

            if (!slot ||
                (slot->generation != Backend::AssetGeneration() &&
                 !Revalidate(*slot))) {
                if (ShouldWarn(WarnReason::NotLoadedOrStale)) {
                    TraceLog(
                        LOG_WARNING,
                        "AnimationPlayer: used while not loaded, or its "
                        "model/animation library was removed. Check that "
                        "Load() succeeded and that the same player object "
                        "(not a copy) is being updated."
                    );
                }

                return nullptr;
            }

            return slot;
        }

        template <typename T>
        static T* CopyArray(const T* source, size_t count) {
            if (!source || count == 0) {
                return nullptr;
            }

            T* destination = static_cast<T*>(
                MemAlloc(static_cast<unsigned int>(sizeof(T) * count))
            );

            if (!destination) {
                return nullptr;
            }

            std::memcpy(destination, source, sizeof(T) * count);
            return destination;
        }

        static bool CopySucceeded(const void* source, const void* copy) {
            return source == nullptr || copy != nullptr;
        }

        // Create a mesh with independent CPU arrays and GPU buffers.
        // Material and skeleton data remain owned by the original model.
        static bool CloneMeshData(const Mesh& source, Mesh& destination) {
            destination = source;

            // Never share GPU handles with the original mesh.
            destination.vaoId = 0;
            destination.vboId = nullptr;

            // Clear all pointers before attempting allocations, so cleanup
            // never frees memory belonging to the original model.
            destination.vertices = nullptr;
            destination.texcoords = nullptr;
            destination.texcoords2 = nullptr;
            destination.normals = nullptr;
            destination.tangents = nullptr;
            destination.colors = nullptr;
            destination.indices = nullptr;

            destination.animVertices = nullptr;
            destination.animNormals = nullptr;
            destination.boneIds = nullptr;
            destination.boneWeights = nullptr;
            destination.boneMatrices = nullptr;

            const size_t vertices = static_cast<size_t>(source.vertexCount);
            const size_t indices =
                static_cast<size_t>(source.triangleCount) * 3;

            destination.vertices =
                CopyArray(source.vertices, vertices * 3);
            destination.texcoords =
                CopyArray(source.texcoords, vertices * 2);
            destination.texcoords2 =
                CopyArray(source.texcoords2, vertices * 2);
            destination.normals =
                CopyArray(source.normals, vertices * 3);
            destination.tangents =
                CopyArray(source.tangents, vertices * 4);
            destination.colors =
                CopyArray(source.colors, vertices * 4);
            destination.indices =
                CopyArray(source.indices, indices);

            destination.animVertices =
                CopyArray(source.animVertices, vertices * 3);
            destination.animNormals =
                CopyArray(source.animNormals, vertices * 3);
            destination.boneIds =
                CopyArray(source.boneIds, vertices * 4);
            destination.boneWeights =
                CopyArray(source.boneWeights, vertices * 4);
            destination.boneMatrices =
                CopyArray(
                    source.boneMatrices,
                    static_cast<size_t>(source.boneCount)
                );

            return
                CopySucceeded(source.vertices, destination.vertices) &&
                CopySucceeded(source.texcoords, destination.texcoords) &&
                CopySucceeded(source.texcoords2, destination.texcoords2) &&
                CopySucceeded(source.normals, destination.normals) &&
                CopySucceeded(source.tangents, destination.tangents) &&
                CopySucceeded(source.colors, destination.colors) &&
                CopySucceeded(source.indices, destination.indices) &&
                CopySucceeded(source.animVertices, destination.animVertices) &&
                CopySucceeded(source.animNormals, destination.animNormals) &&
                CopySucceeded(source.boneIds, destination.boneIds) &&
                CopySucceeded(source.boneWeights, destination.boneWeights) &&
                CopySucceeded(source.boneMatrices, destination.boneMatrices);
        }

        static bool CanUpdateAnimation(const ::Model& model) {
            if (model.meshCount <= 0 || !model.meshes ||
                model.boneCount <= 0 || !model.bindPose) {
                return false;
            }

            // raylib 5.5's UpdateModelAnimation() expects the mesh animation
            // buffers and bone data to be present.
            for (int i = 0; i < model.meshCount; ++i) {
                const Mesh& mesh = model.meshes[i];

                const char* missing = nullptr;

                if (!mesh.vertices) missing = "vertices";
                else if (!mesh.animVertices) missing = "animVertices";
                else if (!mesh.boneIds) missing = "boneIds";
                else if (!mesh.boneWeights) missing = "boneWeights";
                else if (!mesh.boneMatrices) missing = "boneMatrices";
                else if (mesh.boneCount <= 0) missing = "boneCount";
                else if (mesh.normals && !mesh.animNormals) {
                    missing = "animNormals";
                }

                if (missing) {
                    if (ShouldWarn(WarnReason::MissingMeshData)) {
                        TraceLog(
                            LOG_WARNING,
                            "AnimationPlayer: mesh %d has no '%s'; the model "
                            "cannot be skinned by this raylib build, so it "
                            "is drawn in bind pose.",
                            i,
                            missing
                        );
                    }

                    return false;
                }
            }

            return true;
        }

        static bool CloneAnimatedModel(
            const ::Model& source,
            ::Model& destination
        ) {
            if (!CanUpdateAnimation(source)) {
                return false;
            }

            destination = source;
            destination.meshes = nullptr;

            const size_t count =
                static_cast<size_t>(source.meshCount);

            destination.meshes = static_cast<Mesh*>(
                MemAlloc(
                    static_cast<unsigned int>(sizeof(Mesh) * count)
                )
            );

            if (!destination.meshes) {
                return false;
            }

            std::memset(
                destination.meshes,
                0,
                sizeof(Mesh) * count
            );

            for (int i = 0; i < source.meshCount; ++i) {
                Mesh mesh{};

                if (!CloneMeshData(source.meshes[i], mesh)) {
                    UnloadMesh(mesh);

                    for (int j = 0; j < i; ++j) {
                        UnloadMesh(destination.meshes[j]);
                    }

                    MemFree(destination.meshes);
                    destination.meshes = nullptr;

                    return false;
                }

                // Animation updates the GPU vertex buffers, so each player
                // needs independent GPU buffers as well.
                UploadMesh(&mesh, true);

                destination.meshes[i] = mesh;
            }

            return true;
        }

        static void UnloadAnimatedModel(::Model& model) {
            if (model.meshes) {
                for (int i = 0; i < model.meshCount; ++i) {
                    UnloadMesh(model.meshes[i]);
                }

                MemFree(model.meshes);
            }

            // The materials, material textures, skeleton and bind pose are
            // shared with the source model. Do not unload them here.

            model.meshes = nullptr;
            model.meshCount = 0;
        }

        static void QuaternionToAxisAngle(
            Quaternion rotation,
            Vector3& axis,
            float& angleDegrees
        ) {
            const float lengthSquared =
                rotation.x * rotation.x +
                rotation.y * rotation.y +
                rotation.z * rotation.z +
                rotation.w * rotation.w;

            if (lengthSquared < 1.0e-12f) {
                axis = Vector3{ 0.0f, 1.0f, 0.0f };
                angleDegrees = 0.0f;
                return;
            }

            const float inverseLength =
                1.0f / std::sqrt(lengthSquared);

            rotation.x *= inverseLength;
            rotation.y *= inverseLength;
            rotation.z *= inverseLength;
            rotation.w *= inverseLength;

            // q and -q represent the same rotation. Use the shortest angle.
            if (rotation.w < 0.0f) {
                rotation.x = -rotation.x;
                rotation.y = -rotation.y;
                rotation.z = -rotation.z;
                rotation.w = -rotation.w;
            }

            const float w = std::clamp(rotation.w, -1.0f, 1.0f);
            const float halfAngle = std::acos(w);
            const float sinHalfAngle = std::sin(halfAngle);

            angleDegrees = 2.0f * halfAngle * RAD2DEG;

            if (std::abs(sinHalfAngle) < 1.0e-6f) {
                axis = Vector3{ 0.0f, 1.0f, 0.0f };
            } else {
                axis = Vector3{
                    rotation.x / sinHalfAngle,
                    rotation.y / sinHalfAngle,
                    rotation.z / sinHalfAngle
                };
            }
        }

        // Draws the model posed by the player's current animation frame.
        // Returns false (having drawn nothing) if that isn't possible.
        static bool DrawAnimated(
            uint32_t id,
            const Assets::Model& model,
            const Vector3& position,
            const Quaternion& rotation,
            const Vector3& scale
        ) {
            PlayerSlot* slot = Live(id);

            if (!slot) {
                return false;
            }

            if (slot->animationIndex < 0) {
                if (ShouldWarn(WarnReason::NeverPlayed)) {
                    TraceLog(
                        LOG_WARNING,
                        "AnimationPlayer: Draw() with no active animation; "
                        "Play() was never called or was rejected. Drawing "
                        "bind pose."
                    );
                }

                return false;
            }

            ::Model* sourceModel = Backend::GetRaylibModel(model);

            unsigned int animationCount = 0;
            ModelAnimation* animations =
                Backend::RaylibAnimationLib(
                    slot->library,
                    &animationCount
                );

            if (!sourceModel ||
                !animations ||
                static_cast<unsigned int>(slot->animationIndex) >=
                    animationCount) {
                return false;
            }

            const ModelAnimation animation =
                animations[slot->animationIndex];

            if (animation.frameCount <= 0 ||
                !IsModelAnimationValid(*sourceModel, animation)) {
                if (ShouldWarn(WarnReason::BadAnimation)) {
                    TraceLog(
                        LOG_WARNING,
                        "AnimationPlayer: animation does not match the "
                        "model's skeleton (or has no frames). Drawing bind "
                        "pose."
                    );
                }

                return false;
            }

            const float fps = Backend::RaylibAnimationFps(
                slot->library,
                static_cast<unsigned int>(slot->animationIndex)
            );

            const int frame = std::clamp(
                static_cast<int>(slot->elapsedTime * fps),
                0,
                animation.frameCount - 1
            );

            ::Model animatedModel{};

            if (!CloneAnimatedModel(*sourceModel, animatedModel)) {
                // Avoid calling UpdateModelAnimation on a model whose mesh
                // animation buffers are missing or shared.
                if (ShouldWarn(WarnReason::CloneFailed)) {
                    TraceLog(
                        LOG_WARNING,
                        "AnimationPlayer: could not clone the model for "
                        "skinning. Drawing bind pose."
                    );
                }

                return false;
            }

            UpdateModelAnimation(animatedModel, animation, frame);

            Vector3 axis{};
            float angleDegrees = 0.0f;
            QuaternionToAxisAngle(rotation, axis, angleDegrees);

            DrawModelEx(
                animatedModel,
                position,
                axis,
                angleDegrees,
                scale,
                WHITE
            );

            UnloadAnimatedModel(animatedModel);

            return true;
        }

    } // namespace

    AnimationPlayer::AnimationPlayer() = default;

    AnimationPlayer::~AnimationPlayer() {
        Unload();
    }

    // Copying intentionally creates an empty player.
    AnimationPlayer::AnimationPlayer(const AnimationPlayer&) {}

    AnimationPlayer& AnimationPlayer::operator=(const AnimationPlayer&) {
        return *this;
    }

    AnimationPlayer::AnimationPlayer(AnimationPlayer&& other) noexcept
        : m_id(std::exchange(other.m_id, 0)) {}

    AnimationPlayer& AnimationPlayer::operator=(
        AnimationPlayer&& other
    ) noexcept {
        if (this != &other) {
            Unload();
            m_id = std::exchange(other.m_id, 0);
        }

        return *this;
    }

    void AnimationPlayer::Unload() {
        if (PlayerSlot* slot = s_players.Get(m_id)) {
            s_players.Free(m_id);
        }

        m_id = 0;
    }

    bool AnimationPlayer::Load(
        const Assets::Model& model,
        const Assets::AnimationLib& library
    ) {
        Unload();

        ::Model* raylibModel = Backend::GetRaylibModel(model);

        unsigned int animationCount = 0;
        ModelAnimation* animations =
            Backend::RaylibAnimationLib(library, &animationCount);

        if (!raylibModel) {
            if (ShouldWarn(WarnReason::LoadNoModel)) {
                TraceLog(
                    LOG_WARNING,
                    "AnimationPlayer::Load: model does not exist."
                );
            }

            return false;
        }

        if (!animations || animationCount == 0) {
            if (ShouldWarn(WarnReason::LoadNoAnimations)) {
                TraceLog(
                    LOG_WARNING,
                    "AnimationPlayer::Load: animation library does not "
                    "exist or contains no animations."
                );
            }

            return false;
        }

        if (raylibModel->boneCount <= 0) {
            if (ShouldWarn(WarnReason::LoadNoBones)) {
                TraceLog(
                    LOG_WARNING,
                    "AnimationPlayer::Load: model has no skeleton "
                    "(boneCount == 0), nothing to animate."
                );
            }

            return false;
        }

        PlayerSlot slot{};
        slot.model = model;
        slot.library = library;
        slot.generation = Backend::AssetGeneration();
        slot.animationIndex = -1;
        slot.elapsedTime = 0.0f;
        slot.playing = false;
        slot.loop.resize(animationCount, true);

        m_id = s_players.Alloc(std::move(slot));

        return m_id != 0;
    }

    bool AnimationPlayer::IsLoaded() const {
        return s_players.Get(m_id) != nullptr;
    }

    bool AnimationPlayer::IsStale() const {
        PlayerSlot* slot = s_players.Get(m_id);

        // Stale only if the assets changed AND the slot can't be re-synced.
        return slot &&
               slot->generation != Backend::AssetGeneration() &&
               !Revalidate(*slot);
    }

    void AnimationPlayer::SetLoop(int animation, bool loop) {
        PlayerSlot* slot = Live(m_id);

        if (!slot ||
            animation < 0 ||
            static_cast<size_t>(animation) >= slot->loop.size()) {
            return;
        }

        slot->loop[static_cast<size_t>(animation)] = loop;
    }

    void AnimationPlayer::Play(int animation) {
        PlayerSlot* slot = Live(m_id);

        if (!slot || animation < 0) {
            return;
        }

        // Calling Play() with the animation that is already playing must
        // not restart it. Game code commonly calls Play() every frame; if
        // that rewound elapsedTime to 0 each time, the animation would be
        // stuck on frame 0. To restart on purpose: Stop(), then Play().
        if (slot->playing && slot->animationIndex == animation) {
            return;
        }

        unsigned int animationCount = 0;
        ModelAnimation* animations =
            Backend::RaylibAnimationLib(
                slot->library,
                &animationCount
            );

        ::Model* model = Backend::GetRaylibModel(slot->model);

        if (!animations ||
            static_cast<unsigned int>(animation) >= animationCount ||
            animations[animation].frameCount <= 0 ||
            !model ||
            !IsModelAnimationValid(*model, animations[animation])) {
            if (ShouldWarn(WarnReason::PlayRejected)) {
                TraceLog(
                    LOG_WARNING,
                    "AnimationPlayer::Play(%d) rejected: index out of "
                    "range, animation has no frames, or it does not match "
                    "the model's skeleton.",
                    animation
                );
            }

            return;
        }

        slot->animationIndex = animation;
        slot->elapsedTime = 0.0f;
        slot->playing = true;
    }

    void AnimationPlayer::Stop() {
        if (PlayerSlot* slot = Live(m_id)) {
            slot->playing = false;
            slot->elapsedTime = 0.0f;
        }
    }

    void AnimationPlayer::Update(float deltaTime) {
        PlayerSlot* slot = Live(m_id);

        if (!slot) {
            return;
        }

        if (!slot->playing) {
            if (slot->animationIndex < 0 &&
                ShouldWarn(WarnReason::NeverPlayed)) {
                TraceLog(
                    LOG_WARNING,
                    "AnimationPlayer::Update: no animation is playing; "
                    "Play() was never called or was rejected."
                );
            }

            return;
        }

        if (deltaTime <= 0.0f) {
            if (ShouldWarn(WarnReason::ZeroDelta)) {
                TraceLog(
                    LOG_WARNING,
                    "AnimationPlayer::Update called with deltaTime <= 0 "
                    "(ignored). Is the right time value being passed?"
                );
            }

            return;
        }

        unsigned int animationCount = 0;
        ModelAnimation* animations =
            Backend::RaylibAnimationLib(
                slot->library,
                &animationCount
            );

        if (!animations ||
            slot->animationIndex < 0 ||
            static_cast<unsigned int>(slot->animationIndex) >=
                animationCount) {
            slot->playing = false;
            return;
        }

        const int frameCount =
            animations[slot->animationIndex].frameCount;

        if (frameCount <= 0) {
            slot->playing = false;
            return;
        }

        // Per-animation rate recorded when the library was loaded.
        const float fps = Backend::RaylibAnimationFps(
            slot->library,
            static_cast<unsigned int>(slot->animationIndex)
        );

        const float duration = static_cast<float>(frameCount) / fps;

        slot->elapsedTime += deltaTime;

        if (slot->loop[static_cast<size_t>(slot->animationIndex)]) {
            if (duration > 0.0f) {
                slot->elapsedTime =
                    std::fmod(slot->elapsedTime, duration);
            }
        } else if (slot->elapsedTime >= duration) {
            slot->elapsedTime =
                static_cast<float>(frameCount - 1) / fps;

            slot->playing = false;
        }
    }

    void AnimationPlayer::Draw(
        const Assets::Model& model,
        const Vector3& position,
        const Quaternion& rotation,
        const Vector3& scale
    ) const {
        if (DrawAnimated(m_id, model, position, rotation, scale)) {
            return;
        }

        // The player can't produce a pose right now: it's stale, no
        // animation has been played yet, the animation doesn't match the
        // model, or the per-draw mesh clone failed. Draw the model in its
        // bind pose instead of making it vanish.
        Assets::ModelDraw(model, position, rotation, scale);
    }

} // namespace Lil::Render