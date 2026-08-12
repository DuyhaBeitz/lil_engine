#include "Components/LightComponent.hpp"

void LightComponent::UpdateLight() {    
    if (m_enabled) R3D_EnableLight(m_light);
    else R3D_DisableLight(m_light);

    if (m_type != int(R3D_GetLightType(m_light))) {
        R3D_DestroyLight(m_light);
        m_light = R3D_CreateLight(R3D_LightType(m_type));
    }

    if (m_cast_shadows) R3D_EnableShadow(m_light);
    else R3D_DisableShadow(m_light);

    R3D_SetLightRange(m_light, m_range);
    R3D_SetShadowSoftness(m_light, m_shadow_softness);
    R3D_SetShadowDepthBias(m_light, m_depth_bias);

    m_needs_update = false;
}