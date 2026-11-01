#pragma once
#include <cstdint>
#include <r3d/r3d.h>
#include "Assets/Model.hpp"
#include "Assets/AnimationLib.hpp"

namespace Lil::Backend {
    R3D_Model*        R3DModel(const Assets::Model& handle);              // defined in Model.cpp
    R3D_AnimationLib* R3DAnimationLib(const Assets::AnimationLib& handle);// defined in AnimationLib.cpp

    // Bumped whenever a model or animation lib is removed or replaced.
    // Handles built from them compare against it to detect staleness.
    uint32_t AssetGeneration();
    void     BumpAssetGeneration();
}