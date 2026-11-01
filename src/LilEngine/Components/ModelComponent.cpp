#include "Components/ModelComponent.hpp"

#include "LilEngine.hpp"

ModelComponent::ModelComponent(Lil::Assets::Model model_key) : m_model_key(model_key), Component() {}

void ModelComponent::Draw() {
    Component::Draw();
    if (Lil::Assets::ModelExists(m_model_key)) Lil::Assets::ModelDraw(m_model_key, GetPosition(), GetRotation(), GetScale());
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}


void ModelComponent::SetModel(Lil::Assets::Model model_key) {m_model_key = model_key;}

Lil::Assets::Model ModelComponent::ModelKey() { return m_model_key; }


RayCollision ModelComponent::Raycast(Ray ray) const {
    return Lil::Assets::ModelRaycast(m_model_key, ray, GetTransform());
}