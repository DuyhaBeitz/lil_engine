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

Model *ModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &ModelComponent::ModelKey() { return m_model_key; }


RayCollision ModelComponent::Raycast(Ray ray) const {
    Model* m = GetModel();
    
    RayCollision res;
    
    if (m) {
        for (int i = 0; i < m->meshCount; i++) {
            Vector3 s = GetScale();
            Matrix matScale = MatrixScale(s.x, s.y, s.z);

            float angle;
            Vector3 axis = GetAxisAngle(angle);
            Matrix matRotation = MatrixRotate(axis, angle);

            Vector3 p = GetPosition();
            Matrix matTranslation = MatrixTranslate(p.x, p.y, p.z);

            Matrix mx = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);

            //DrawMesh(m->meshes[i], m->materials[0], mx);

            RayCollision r = GetRayCollisionMesh(ray, m->meshes[i], MatrixMultiply(m->transform, mx));

            if (r.hit) {
                return r;
                DrawSphere(r.point, 10, RED);
            }
        }
    }
    return res;
}