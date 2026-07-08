#include "Actor.hpp"

#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "Physics.hpp"

Actor::Actor(rc::BodyType body_type, Vector3 position) : m_body(Physics::Get().GetWorld()->createRigidBody(rc::Transform{})) {
    m_body->setType(body_type);
    SetPosition(position);
}

Vector3 Actor::GetAxisAngle(float &angle)
{
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