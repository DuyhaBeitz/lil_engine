#pragma once
#include "CommonIncludes.hpp"

inline Vector3 QuaternionToAxis(const rc::Quaternion& q, float& angleDeg) {
    rc::Quaternion nq = q;
    // Normalize quaternion just in case
    float len = std::sqrt(nq.x*nq.x + nq.y*nq.y + nq.z*nq.z + nq.w*nq.w);
    nq.x /= len; nq.y /= len; nq.z /= len; nq.w /= len;

    angleDeg = 2.0f * std::acos(nq.w) * (180.0f / PI); // convert to degrees
    float s = std::sqrt(1.0f - nq.w*nq.w);

    Vector3 axis;
    if (s < 0.001f) { // if s is close to zero, direction is not important
        axis = {1.0f, 0.0f, 0.0f};
    } else {
        axis.x = nq.x / s;
        axis.y = nq.y / s;
        axis.z = nq.z / s;
    }

    return axis;
}

inline rc::Vector3 RcVector3(Vector3 v) {
    return rc::Vector3(v.x, v.y, v.z);
}
inline rc::Quaternion RcQuaternion(Quaternion q) {
    return rc::Quaternion(q.x, q.y, q.z, q.w);
}
inline rc::Transform RcTransform(Transform t) {
    return rc::Transform(RcVector3(t.translation), RcQuaternion(t.rotation));
}

inline Vector3 RlVector3(rc::Vector3 v) {
    return Vector3{v.x, v.y, v.z};
}
inline Quaternion RlQuaternion(rc::Quaternion q) {
    return Quaternion{q.x, q.y, q.z, q.w};
}

inline Transform RlTransform(rc::Transform t) {
    return Transform{
        .translation = RlVector3(t.getPosition()),
        .rotation = RlQuaternion(t.getOrientation()),
        .scale = Vector3{1.0f, 1.0f, 1.0f}
    };
}