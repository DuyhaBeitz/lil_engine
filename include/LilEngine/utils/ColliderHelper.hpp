#pragma once

#include "CommonIncludes.hpp"
#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "Physics.hpp"

void AddBoundsBoxColliderToBody(rc::RigidBody* body, const Model& model);
void AddBoundsBoxColliderToBodyMulty(rc::RigidBody* body, const Model& model);
void AddBoundingSphereColliderToBody(rc::RigidBody* body, const Model& model);
void AddHeightmapCollider(Image heightmap_image, Vector3 scale, rc::RigidBody* body);