#include "Pawn.hpp"
#include "ResourceManager.hpp"
#include "utils/ColliderHelper.hpp"

void Pawn::Draw() {
    float angle;
    Vector3 axis = GetAxisAngle(angle);
    Model* m = GetModel();
    if (m) DrawModelEx(*m, GetPosition()+m_model_local_offset, axis, angle, Vector3{1,1,1}, RAYWHITE);
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}

void Pawn::SetModel(std::string model_key, Vector3 local_offset) {
    m_model_key = model_key;
    m_model_local_offset = local_offset;
}

Model *Pawn::GetModel() { return ResourceManager::Get().GetModel(m_model_key); }
std::string &Pawn::ModelKey() {return m_model_key;}

void Pawn::AddBoundBoxCollider() {
    AddBoundsBoxColliderToBody(m_body, *ResourceManager::Get().GetModel(m_model_key));
}

void Pawn::AddBoundBoxColliderMulty() {
    AddBoundsBoxColliderToBodyMulty(m_body, *ResourceManager::Get().GetModel(m_model_key));
}

void Pawn::AddBoundSphereCollider() {
    AddBoundingSphereColliderToBody(m_body, *ResourceManager::Get().GetModel(m_model_key));
}


