#include "Component.hpp"
#include "utils/MathHelper.hpp"

Transformable::Transformable(Transform transform) : m_transform(transform), Reflectable() {}

void Transformable::SetTransform(Transform transform) {m_transform = transform;}
void Transformable::SetPosition(Vector3 position) {m_transform.translation = position;}
void Transformable::SetRotation(Quaternion rotation) {m_transform.rotation = rotation;}
void Transformable::SetScale(Vector3 scale) {m_transform.scale = scale;}

Transform Transformable::GetTransform() const {return m_transform;}
Vector3 Transformable::GetPosition() const {return m_transform.translation;}
Quaternion Transformable::GetRotation() const {return m_transform.rotation;}
Vector3 Transformable::GetScale() const {return m_transform.scale;}

Vector3 Transformable::GetAxisAngle(float &angle) const {
    Vector3 axis;
    QuaternionToAxisAngle(m_transform.rotation, &axis, &angle);
    return axis;
    //return QuaternionToAxis(RcQuaternion(m_transform.rotation), angle);
}

Component::Component(Transform local_transform)
 : m_local_transform(local_transform), GameObject() {}

void Component::ApplyParentTransform(Transform parent_transform)
{
    SetTransform(ApplyLocalTransform(parent_transform, m_local_transform));
}

void Component::LayoutUpdate() {}
void Component::SimulationUpdate(Actor& actor, float delta_time) {}
void Component::Draw() {}
void Component::DebugUpdate() {}
void Component::DebugDraw() {}

void Component::MarkRequired() { m_is_required = true; }
bool Component::IsRequired() { return m_is_required; }

RayCollision Component::Raycast(Ray ray) const {
    return RayCollision{0};
}