#include "Render/Light.hpp"
#include <utility>
#include <lil3d/lil3d.h>
#include "Render/Pool.hpp"

namespace Lil::Render {
    struct LightSlot { LIL3D_Light light; LightType type; };
    static Pool<LightSlot> s_lights;

    static LIL3D_LightType ToBackendType(LightType t) {
        switch (t) {
            case LightType::Directional: return LIL3D_LIGHT_DIR;
            case LightType::Omni:        return LIL3D_LIGHT_OMNI;
            default:                     return LIL3D_LIGHT_SPOT;
        }
    }

    static void Destroy(uint32_t id) {
        if (LightSlot* s = s_lights.Get(id)) {
            LIL3D_DestroyLight(s->light);
            s_lights.Free(id);
        }
    }

    Light::Light() = default;
    Light::~Light() { Destroy(m_id); }
    Light::Light(const Light&) {}                           // empty on copy
    Light& Light::operator=(const Light&) { return *this; } // keep own light
    Light::Light(Light&& o) noexcept : m_id(std::exchange(o.m_id, 0)) {}
    Light& Light::operator=(Light&& o) noexcept {
        if (this != &o) { Destroy(m_id); m_id = std::exchange(o.m_id, 0); }
        return *this;
    }

    void Light::Sync(const LightDesc& d) {
        LightSlot* s = s_lights.Get(m_id);
        if (s && s->type != d.type) {                       // type change: recreate FIRST
            Destroy(m_id); m_id = 0; s = nullptr;
        }
        if (!s) {
            LIL3D_Light l = LIL3D_CreateLight(ToBackendType(d.type));
            if (l == 0) return;                             // backend not ready / out of slots
            m_id = s_lights.Alloc(LightSlot{l, d.type});
            s = s_lights.Get(m_id);
        }
        LIL3D_Light l = s->light;

        LIL3D_SetLightActive(l, d.enabled);
        if (d.cast_shadows) LIL3D_EnableShadow(l); else LIL3D_DisableShadow(l);

        LIL3D_SetLightEnergy(l, d.lumen);
        LIL3D_SetLightColor(l, d.color);
        LIL3D_SetLightRange(l, d.range);
        LIL3D_SetShadowSoftness(l, d.shadow_softness);
        LIL3D_SetShadowBias(l, d.depth_bias);

        if (d.type != LightType::Directional) LIL3D_SetLightPosition(l, d.position);
        if (d.type != LightType::Omni)        LIL3D_SetLightDirection(l, d.direction);
    }

    void Light::DebugDraw() const {
        if (LightSlot* s = s_lights.Get(m_id)) LIL3D_DrawLightShape(s->light);
    }
}
