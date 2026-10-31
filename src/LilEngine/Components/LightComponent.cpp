#include "Components/LightComponent.hpp"

void LightComponent::LayoutUpdate() {
    Component::LayoutUpdate();
    UpdateLight();
}

void LightComponent::DebugDraw() {
    Component::DebugDraw();
    m_light.DebugDraw();
}

void LightComponent::UpdateLight() {
    m_light.Sync(Lil::Render::LightDesc{
        .type            = Lil::Render::LightType(m_type),
        .enabled         = m_enabled,
        .lumen           = m_lumen,
        .color           = m_color,
        .range           = m_range,
        .cast_shadows    = m_cast_shadows,
        .shadow_softness = m_shadow_softness,
        .depth_bias      = m_depth_bias,
        .position        = GetPosition(),
        .direction       = Vector3RotateByQuaternion({0, -1, 0}, GetRotation()),
    });
}