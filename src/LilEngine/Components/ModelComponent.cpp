#include "Components/ModelComponent.hpp"

#include "LilEngine.hpp"
#include "utils/MeshHelper.hpp"

ModelComponent::ModelComponent(std::string model_key)
: m_model_key(model_key), Component()
{
}

void ModelComponent::Draw() {
    Component::Draw();
    if (R3D_Model* m = GetModel()) R3D_DrawModelEx(*m, GetPosition(), GetRotation(), GetScale());
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}


void ModelComponent::SetModel(std::string model_key) {m_model_key = model_key;}
R3D_Model *ModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &ModelComponent::ModelKey() { return m_model_key; }


RayCollision ModelComponent::Raycast(Ray ray) const {
    RayCollision res = { 0 };
    
    if (R3D_Model* m = GetModel()) {
        Vector3 s = GetScale();
        Matrix matScale = MatrixScale(s.x, s.y, s.z);

        float angle;
        Vector3 axis = GetAxisAngle(angle);
        Matrix matRotation = MatrixRotate(axis, angle);

        Vector3 p = GetPosition();
        Matrix matTranslation = MatrixTranslate(p.x, p.y, p.z);

        Matrix mx = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

        res = R3D_RaycastModel(ray, *m, mx);
    }
    
    return res;
}