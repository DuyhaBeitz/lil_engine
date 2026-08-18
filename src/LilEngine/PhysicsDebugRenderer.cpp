#include "PhysicsDebugRenderer.hpp"

void RaylibJoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {
    DrawLine3D(
        Vector3{ (float)inFrom.GetX(), (float)inFrom.GetY(), (float)inFrom.GetZ() },
        Vector3{ (float)inTo.GetX(), (float)inTo.GetY(), (float)inTo.GetZ() },
        Color{ inColor.r, inColor.g, inColor.b, inColor.a }
    );
}

void RaylibJoltDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) {
    Color c = Color{ inColor.r, inColor.g, inColor.b, (unsigned char)(inColor.a * 0.5f) };
    
    Vector3 points[3] = {
        Vector3{ (float)inV1.GetX(), (float)inV1.GetY(), (float)inV1.GetZ() },
        Vector3{ (float)inV2.GetX(), (float)inV2.GetY(), (float)inV2.GetZ() },
        Vector3{ (float)inV3.GetX(), (float)inV3.GetY(), (float)inV3.GetZ() }
    };

    DrawTriangle3D(points[0], points[1], points[2], c);
}

void RaylibJoltDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const std::string_view &inString, JPH::ColorArg inColor, float inHeight) {}
