#include "Components/ModelComponent.hpp"

#include "LilEngine.hpp"
#include "utils/MeshHelper.hpp"

ModelComponent::ModelComponent(std::string model_key)
: m_model_key(model_key), Component()
{
}

void ModelComponent::Draw() {
    R3D_Model* m = GetModel();
    if (m) {
    switch (Lil::World().GetRenderMode()) {
        case RenderMode::Unlit:
            R3D_DrawModelEx(*m, GetPosition(), GetRotation(), GetScale());
            break;
        
        case RenderMode::Wireframe:
            DrawR3DModelWiresEx(*m, GetPosition(), GetRotation(), GetScale());
            break;
        }
    }
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}


void ModelComponent::SetModel(std::string model_key) {m_model_key = model_key;}
R3D_Model *ModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &ModelComponent::ModelKey() { return m_model_key; }


RayCollision ModelComponent::Raycast(Ray ray) const {
    R3D_Model* m = GetModel();
    
    RayCollision res = { 0 };
    
    if (m) {
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