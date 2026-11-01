#include "Render/AnimationPlayer.hpp"
#include <utility>
#include <lil3d/lil3d.h>
#include "Render/Pool.hpp"
#include "BackendAccess.hpp"

namespace Lil::Render {

    struct PlayerSlot { LIL3D_AnimationPlayer player; uint32_t generation; };
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
            LIL3D_UnloadAnimationPlayer(s->player);
            s_players.Free(m_id);
        }
        m_id = 0;
    }

    bool AnimationPlayer::Load(const Assets::Model& model, const Assets::AnimationLib& lib) {
        Unload();
        LIL3D_Model* m = Backend::GetRaylibModel(model);
        LIL3D_AnimationLib* l = Backend::RaylibAnimationLib(lib);
        if (!m || !l) return false;
        LIL3D_AnimationPlayer p = LIL3D_LoadAnimationPlayer(*m, *l);
        if (!LIL3D_IsAnimationPlayerValid(p)) return false;    // no skeleton / empty lib
        m_id = s_players.Alloc(PlayerSlot{ p, Backend::AssetGeneration() });
        return true;
    }

    bool AnimationPlayer::IsLoaded() const { return s_players.Get(m_id) != nullptr; }
    bool AnimationPlayer::IsStale()  const { return s_players.Get(m_id) && !Live(m_id); }

    static LIL3D_AnimationState* State(PlayerSlot* s, int anim) {
        return (s && anim >= 0 && anim < s->player.stateCount) ? &s->player.states[anim] : nullptr;
    }

    void AnimationPlayer::SetLoop(int anim, bool loop) {
        if (auto* st = State(Live(m_id), anim)) st->loop = loop;
    }

    // Exclusive play. Calling it again for the animation that is already the only
    // one playing is a no-op, so it is safe to call every frame.
    void AnimationPlayer::Play(int anim) {
        PlayerSlot* s = Live(m_id);
        LIL3D_AnimationState* st = State(s, anim);
        if (!st) return;
        bool only = st->playing && st->weight > 0.0f;
        for (int i = 0; only && i < s->player.stateCount; i++)
            if (i != anim && (s->player.states[i].playing || s->player.states[i].weight > 0.0f)) only = false;
        if (only) return;
        LIL3D_PlayAnimation(&s->player, anim, st->loop);
    }

    void AnimationPlayer::Stop()           { if (auto* s = Live(m_id)) LIL3D_StopAnimations(&s->player); }
    void AnimationPlayer::Update(float dt) { if (auto* s = Live(m_id)) LIL3D_UpdateAnimationPlayer(&s->player, dt); }

    void AnimationPlayer::Draw(const Assets::Model& model, const Vector3& pos,
                               const Quaternion& rot, const Vector3& scale) const {
        PlayerSlot* s = Live(m_id);
        LIL3D_Model* m = Backend::GetRaylibModel(model);
        if (s && m) LIL3D_DrawAnimatedModelPro(*m, &s->player, Backend::ComposeTransform(pos, rot, scale));
    }
}
