#include "utils/MathHelper.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h> 
#include <raymath.h>

Vector3 QuaternionToAxis(const JPH::Quat &q, float &angleDeg) {
    JPH::Quat nq = q.Normalized();

    angleDeg = 2.0f * std::acos(nq.GetW()) * (180.0f / PI); // convert to degrees
    float s = std::sqrt(1.0f - nq.GetW()*nq.GetW());

    Vector3 axis;
    if (s < 0.001f) {
        axis = {1.0f, 0.0f, 0.0f};
    } else {
        axis.x = nq.GetX() / s;
        axis.y = nq.GetY() / s;
        axis.z = nq.GetZ() / s;
    }
    return axis;
}

Transform ApplyLocalTransform(Transform global, Transform local) {
    Vector3 rotated_pos = Vector3RotateByQuaternion(local.translation, global.rotation);
    global.translation = global.translation + rotated_pos;
    global.rotation = QuaternionMultiply(global.rotation, local.rotation);
    global.scale = local.scale * global.scale;
    return global;
}

Transform ApplyInverseLocalTransform(Transform global, Transform local) {
    Transform result = global;

    result.rotation = QuaternionMultiply(global.rotation, QuaternionInvert(local.rotation));
    result.translation = global.translation - Vector3RotateByQuaternion(local.translation, result.rotation);
    result.scale = global.scale / local.scale;

    return result;
}

Transform GetLocalTransform(Transform parent, Transform child) {
    Transform result;

    result.rotation =
        QuaternionMultiply(
            QuaternionInvert(parent.rotation),
            child.rotation
        );

    Vector3 worldOffset =
        child.translation - parent.translation;

    result.translation =
        Vector3RotateByQuaternion(
            worldOffset,
            QuaternionInvert(parent.rotation)
        );

    result.scale =
        child.scale / parent.scale;

    return result;
}

JPH::Vec3 JphVector3(const Vector3 &v) {return JPH::Vec3(v.x, v.y, v.z);}
Vector3 RlVector3(const JPH::Vec3& v) {return Vector3{ v.GetX(), v.GetY(), v.GetZ() };}
JPH::Quat JphQuat(const Quaternion& q) {return JPH::Quat(q.x, q.y, q.z, q.w).Normalized();}
Quaternion RlQuat(const JPH::Quat& q) {return QuaternionNormalize(Quaternion{ q.GetX(), q.GetY(), q.GetZ(), q.GetW()});}
