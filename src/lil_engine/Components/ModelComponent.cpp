#include "Components/ModelComponent.hpp"

#include "lil_engine.hpp"

ModelComponent::ModelComponent(std::string model_key)
: m_model_key(model_key), Component()
{
}

void ModelComponent::Draw() {
    float angle;
    Vector3 axis = GetAxisAngle(angle);
    Model* m = GetModel();
    if (m) DrawModelEx(*m, GetPosition(), axis, angle*RAD2DEG, GetScale(), WHITE);
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}


void ModelComponent::SetModel(std::string model_key) {
    m_model_key = model_key;
}

Model *ModelComponent::GetModel() { return Lil::Resources().GetModel(m_model_key); }
std::string &ModelComponent::ModelKey() {return m_model_key;}