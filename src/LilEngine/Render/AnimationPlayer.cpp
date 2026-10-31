#include "Render/AnimationPlayer.hpp"
#include <utility>
#include <r3d/r3d.h>
#include "Render/Pool.hpp"
#include "../AdvancedBackend/R3DAccess.hpp"

namespace Lil::Render {

    struct PlayerSlot { R3D_AnimationPlayer player; uint32_t generation; };
    static Pool<PlayerSlot> s_players;

    static PlayerSlot* Live(uint32_t id) {            // nullptr if empty or stale
        PlayerSlot* s = s_players.Get(id);
        return (s && s->generation == Backend::AssetGeneration()) ? s : nullptr;
    }

    AnimationPlayer::AnimationPlayer() = default;
    AnimationPlayer::~AnimationPlayer() { Unload(); }
    AnimationPlayer::AnimationPlayer(const AnimationPlayer&) {}
    AnimationPlayer& AnimationPlayer::operator=(const AnimationPlayer&) { return *this; }
    AnimationPlayer::AnimationPlayer(AnimationPlayer&& o) noexcept : m_id(std::exchange(o.m_id, 0)) {}
    AnimationPlayer& AnimationPlayer::operator=(AnimationPlayer&& o) noexcept {
        if (this != &o) { Unload(); m_id = std::exchange(o.m_id, 0); }
        return *this;
    }

    void AnimationPlayer::Unload() {
        if (PlayerSlot* s = s_players.Get(m_id)) {
            R3D_UnloadAnimationPlayer(s->player);
            s_players.Free(m_id);
        }
        m_id = 0;
    }

    bool AnimationPlayer::Load(const Assets::Model& model, const Assets::AnimationLib& lib) {
        Unload();
        R3D_Model* m = Backend::R3DModel(model);
        R3D_AnimationLib* l = Backend::R3DAnimationLib(lib);
        if (!m || !l) return false;
        m_id = s_players.Alloc(PlayerSlot{ R3D_LoadAnimationPlayer(m->skeleton, *l),
                                           Backend::AssetGeneration() });
        return true;
    }

    bool AnimationPlayer::IsLoaded() const { return s_players.Get(m_id) != nullptr; }
    bool AnimationPlayer::IsStale()  const { return s_players.Get(m_id) && !Live(m_id); }

    void AnimationPlayer::SetLoop(int anim, bool loop) { if (auto* s = Live(m_id)) R3D_SetAnimationLoop(&s->player, anim, loop); }
    void AnimationPlayer::Play(int anim)               { if (auto* s = Live(m_id)) R3D_PlayAnimation(&s->player, anim); }
    void AnimationPlayer::Stop()                       { if (auto* s = Live(m_id)) R3D_StopAnimation(&s->player); }
    void AnimationPlayer::Update(float dt)             { if (auto* s = Live(m_id)) R3D_UpdateAnimationPlayer(&s->player, dt); }

    void AnimationPlayer::Draw(const Assets::Model& model, const Vector3& pos,
                               const Quaternion& rot, const Vector3& scale) const {
        PlayerSlot* s = Live(m_id);
        R3D_Model* m = Backend::R3DModel(model);
        if (s && m) R3D_DrawAnimatedModelEx(*m, s->player, pos, rot, scale);
    }
}