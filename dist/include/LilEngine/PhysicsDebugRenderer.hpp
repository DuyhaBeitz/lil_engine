#pragma once
#include "CommonIncludes.hpp"
#include "JoltWrapper.hpp"

class RaylibJoltDebugRenderer final : public JPH::DebugRendererSimple {
public:
    RaylibJoltDebugRenderer() {
        // Initialize the renderer using standard Jolt font routines if text is needed
        Initialize();
    }

    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
};