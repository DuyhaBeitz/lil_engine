#pragma once
#include <cstdint>
#include <raylib.h>

namespace Lil::Render {

    enum class LightType : int { Directional = 0, Spot = 1, Omni = 2 };

    struct LightDesc {
        LightType type = LightType::Directional;
        bool  enabled = true;
        float lumen = 1.0f;
        Color color = {255, 255, 255, 255};
        float range = 10.0f;
        bool  cast_shadows = true;
        float shadow_softness = 1.0f;
        float depth_bias = 0.001f;
        Vector3 position  = {0, 0, 0};
        Vector3 direction = {0, -1, 0};
    };

    // RAII handle. The backend light is created lazily on the first Sync().
    // Copying yields an EMPTY light (never shared), recreated on the next Sync().
    class Light {
    public:
        Light();
        ~Light();
        Light(const Light&);
        Light& operator=(const Light&);
        Light(Light&&) noexcept;
        Light& operator=(Light&&) noexcept;

        void Sync(const LightDesc& desc);    // creates, recreates on type change, updates
        void DebugDraw() const;
        bool IsValid() const { return m_id != 0; }
    private:
        uint32_t m_id = 0;
    };
}