#include "Render/Light.hpp"
#include <utility>
#include <r3d/r3d.h>
#include "Render/Pool.hpp"

namespace Lil::Render {
    struct LightSlot { R3D_Light light; LightType type; };
    static Pool<LightSlot> s_lights;

    static void Destroy(uint32_t id) {
        if (LightSlot* s = s_lights.Get(id)) {
            R3D_DestroyLight(s->light);
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
            R3D_Light l = R3D_CreateLight(R3D_LightType(d.type));
            R3D_SetShadowUpdateMode(l, R3D_SHADOW_UPDATE_CONTINUOUS);
            m_id = s_lights.Alloc(LightSlot{l, d.type});
            s = s_lights.Get(m_id);
        }
        R3D_Light l = s->light;

        if (d.enabled) R3D_EnableLight(l); else R3D_DisableLight(l);
        if (d.cast_shadows) R3D_EnableShadow(l); else R3D_DisableShadow(l);

        R3D_SetLightLumen(l, d.lumen);
        R3D_SetLightColor(l, d.color);
        R3D_SetLightRange(l, d.range);
        R3D_SetShadowSoftness(l, d.shadow_softness);
        R3D_SetShadowDepthBias(l, d.depth_bias);

        if (d.type != LightType::Directional) R3D_SetLightPosition(l, d.position);
        if (d.type != LightType::Omni)        R3D_SetLightDirection(l, d.direction);
    }

    void Light::DebugDraw() const {
        if (LightSlot* s = s_lights.Get(m_id)) R3D_DrawLightDebug(s->light);
    }
}