#include "Actor.hpp"

#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "ResourceManager.hpp"

void Actor::SetModel(std::string model_key, Vector3 local_offset) {
    m_model_key = model_key;
    m_model_local_offset = local_offset;
}

void Actor::Draw() {
    float angle;
    Vector3 axis = GetAxisAngle(angle);
    Model* m = GetModel();
    if (m) DrawModelEx(*m, GetPosition()+m_model_local_offset, axis, angle, Vector3{1,1,1}, RAYWHITE);
    else DrawSphere(GetPosition(), 2.f, RAYRED);
}

Vector3 Actor::GetAxisAngle(float &angle) {
    const rc::Transform& transform = m_body->getTransform();
    const rc::Quaternion& q = transform.getOrientation();
    return QuaternionToAxis(q, angle);
}

Vector3 Actor::GetPosition() {
    const rc::Transform& transform = m_body->getTransform();
    const rc::Vector3& position = transform.getPosition();
    return Vector3{position.x, position.y, position.z};
}

void Actor::SetPosition(Vector3 position) {
    auto transform = m_body->getTransform();
    transform.setPosition(rc::Vector3{position.x, position.y, position.z});
    m_body->setTransform(transform);
}

rc::RigidBody *Actor::GetBody() { return m_body; }

Model *Actor::GetModel() { return ResourceManager::Get().GetModel(m_model_key); }

std::string &Actor::ModelKey() {return m_model_key;}