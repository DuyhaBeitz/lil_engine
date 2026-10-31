#pragma once
#include "Component.hpp"
#include "Render/Light.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"

class LightComponent : public Component {
public:
    Lil::Render::Light m_light;
    bool m_enabled = true;
    float m_lumen = 1.0f;
    Color m_color = Color{255, 255, 255, 255};
    int m_type = int(Lil::Render::LightType::Directional);
    bool m_cast_shadows = true;
    float m_range = 10.0f;
    float m_shadow_softness = 1.0f;
    float m_depth_bias = 0.001f;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    virtual void LayoutUpdate() override;
    virtual void DebugDraw() override;
    void UpdateLight();
};
LIL_REFLECT(LightComponent, bases<Component>,
    field(m_type, EnumAttribute{
        EnumAttrItem{"Directional", int(Lil::Render::LightType::Directional)},
        EnumAttrItem{"Conus",       int(Lil::Render::LightType::Spot)},
        EnumAttrItem{"Sphere",      int(Lil::Render::LightType::Omni)},
    }),
    field(m_enabled),
    field(m_lumen),
    field(m_color),
    field(m_cast_shadows),
    field(m_range),
    field(m_shadow_softness),
    field(m_depth_bias)
)

LIL_SER_BEGIN(LightComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_enabled)
LIL_SER_FIELD(m_lumen)
LIL_SER_FIELD(m_color)
LIL_SER_FIELD(m_type)
LIL_SER_FIELD(m_cast_shadows)
LIL_SER_FIELD(m_range)
LIL_SER_FIELD(m_shadow_softness)
LIL_SER_FIELD(m_depth_bias)
LIL_SER_END()
