#include "Components/LightComponent.hpp"

LightComponent::LightComponent() {
    m_light = R3D_CreateLight(R3D_LightType(m_type));
    R3D_SetShadowUpdateMode(m_light, R3D_SHADOW_UPDATE_INTERVAL);
}

LightComponent::~LightComponent() { R3D_DestroyLight(m_light); }

void LightComponent::LayoutUpdate() {
    Component::LayoutUpdate();
    
    if (m_type != int(R3D_LIGHT_DIR))
        R3D_SetLightPosition(m_light, GetPosition());
    if (m_type != int(R3D_LIGHT_OMNI))
        R3D_SetLightDirection(m_light, Vector3RotateByQuaternion({0,-1,0}, GetRotation()));
    UpdateLight();
};

void LightComponent::DebugDraw() {
    Component::DebugDraw();
    
    R3D_DrawLightDebug(m_light);
}

void LightComponent::UpdateLight() {
    if (m_enabled) R3D_EnableLight(m_light);
    else R3D_DisableLight(m_light);

    if (m_type != int(R3D_GetLightType(m_light))) {
        R3D_DestroyLight(m_light);
        m_light = R3D_CreateLight(R3D_LightType(m_type));
    }

    if (m_cast_shadows) R3D_EnableShadow(m_light);
    else R3D_DisableShadow(m_light);

    R3D_SetLightLumen(m_light, m_lumen);
    R3D_SetLightColor(m_light, m_color);
    R3D_SetLightRange(m_light, m_range);
    R3D_SetShadowSoftness(m_light, m_shadow_softness);
    R3D_SetShadowDepthBias(m_light, m_depth_bias);

    m_needs_update = false;
}