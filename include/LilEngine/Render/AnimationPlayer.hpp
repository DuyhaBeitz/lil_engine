#pragma once
#include <cstdint>
#include <raylib.h>
#include "Assets/Model.hpp"
#include "Assets/AnimationLib.hpp"

namespace Lil::Render {
    // Copying yields an empty player; call Load() again.
    class AnimationPlayer {
    public:
        AnimationPlayer();
        ~AnimationPlayer();
        AnimationPlayer(const AnimationPlayer&);
        AnimationPlayer& operator=(const AnimationPlayer&);
        AnimationPlayer(AnimationPlayer&&) noexcept;
        AnimationPlayer& operator=(AnimationPlayer&&) noexcept;

        // frees the old one first
        bool Load(const Assets::Model& model, const Assets::AnimationLib& lib);
        void Unload();
        bool IsLoaded() const;
        bool IsStale() const;      // model/lib was unloaded or replaced since Load()

        void SetLoop(int anim, bool loop);
        void Play(int anim);
        void Stop();
        void Update(float delta_time);
        void Draw(const Assets::Model& model, const Vector3& pos,
                  const Quaternion& rot, const Vector3& scale) const;
    private:
        uint32_t m_id = 0;
    };
}