#pragma once
#include <raylib.h>

namespace JPH {
    class Vec3;
    class Quat;
}

Vector3 QuaternionToAxis(const JPH::Quat& q, float& angleDeg);
Transform ApplyLocalTransform(Transform global, Transform local);
Transform ApplyInverseLocalTransform(Transform global, Transform local);
Transform GetLocalTransform(Transform parent, Transform child);

JPH::Vec3 JphVector3(const Vector3& v);
Vector3 RlVector3(const JPH::Vec3& v);
JPH::Quat JphQuat(const Quaternion& q);
Quaternion RlQuat(const JPH::Quat& q);