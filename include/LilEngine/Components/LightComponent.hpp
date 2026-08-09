#pragma once

#include "Component.hpp"

#include "ReflAttributes.hpp"

class LightComponent : public Component {
private:
    virtual void OnLayoutUpdate() override {
        if (m_type != int(R3D_LIGHT_DIR))
            R3D_SetLightPosition(m_light, GetPosition());
        if (m_type != int(R3D_LIGHT_OMNI))
            R3D_SetLightDirection(m_light, Vector3RotateByQuaternion({0,-1,0}, GetRotation()));
        UpdateLight();
    };

public:
    R3D_Light m_light;
    int m_type = int(R3D_LIGHT_DIR);
    bool m_active = true;
    bool m_cast_shadows = true;
    float m_range = 500.0f;
    float m_shadow_softness = 3.2f;
    float m_depth_bias = 0.001f;

    bool m_needs_update = true;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    void UpdateLight();

    LightComponent() {
        m_light = R3D_CreateLight(R3D_LightType(m_type));
        R3D_SetShadowUpdateMode(m_light, R3D_SHADOW_UPDATE_INTERVAL);
    }
    ~LightComponent() {R3D_DestroyLight(m_light);}
};
LIL_REFLECT(LightComponent, bases<Component>,
    field(m_type, EnumAttribute{
        EnumAttrItem{"Directional", 0},
        EnumAttrItem{"Conus", 1},
        EnumAttrItem{"Sphere", 2},
    }),
    field(m_active),
    field(m_cast_shadows),
    field(m_range),
    field(m_shadow_softness),
    field(m_depth_bias)
)

LIL_SER_BEGIN(LightComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_type)
LIL_SER_FIELD(m_active)
LIL_SER_FIELD(m_cast_shadows)
LIL_SER_FIELD(m_range)
LIL_SER_FIELD(m_shadow_softness)
LIL_SER_FIELD(m_depth_bias)
LIL_SER_END()
