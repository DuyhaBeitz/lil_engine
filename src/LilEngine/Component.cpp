#include "Component.hpp"
#include "utils/MathHelper.hpp"

Vector3 Transformable::GetAxisAngle(float &angle) const {
    Vector3 axis;
    QuaternionToAxisAngle(m_transform.rotation, &axis, &angle);
    return axis;
    //return QuaternionToAxis(RcQuaternion(m_transform.rotation), angle);
}