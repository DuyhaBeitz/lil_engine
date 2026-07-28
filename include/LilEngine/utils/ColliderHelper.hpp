#pragma once

#include "CommonIncludes.hpp"
#include "Physics.hpp"

JPH::HeightFieldShapeSettings GetHeightmapShapeSettings(const Image& img, const Vector3 scale = Vector3{1.0f, 1.0f, 1.0f}, const Vector3 offset = Vector3{0.0f, 0.0f, 0.0f});

// void AddBoundsBoxColliderToBody(rc::RigidBody* body, const Model& model);
// void AddBoundsBoxColliderToBodyMulty(rc::RigidBody* body, const Model& model);
// void AddBoundingSphereColliderToBody(rc::RigidBody* body, const Model& model);
// reactphysics3d::HeightFieldShape* CreateHeightmapShape(Image heightmap_image, Vector3 scale);
//reactphysics3d::Collider* AddHeightmapCollider(Image heightmap_image, Vector3 scale, rc::RigidBody* body);