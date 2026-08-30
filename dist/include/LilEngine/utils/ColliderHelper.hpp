#pragma once

#include "CommonIncludes.hpp"
#include "Physics.hpp"

JPH::HeightFieldShapeSettings GetHeightmapShapeSettings(const Image& img, const Vector3 scale = Vector3{1.0f, 1.0f, 1.0f}, const Vector3 offset = Vector3{0.0f, 0.0f, 0.0f});
void DrawDebugPhysicsBody(JPH::BodyID body_id);