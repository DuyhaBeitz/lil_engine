#pragma once
#include <cstdint>
#include <lil3d/lil3d.h>
#include "Assets/Model.hpp"
#include "Assets/AnimationLib.hpp"
#include <raymath.h>

namespace Lil::Backend {
    LIL3D_Model*        GetRaylibModel(const Assets::Model& handle);              // defined in Model.cpp
    LIL3D_AnimationLib* RaylibAnimationLib(const Assets::AnimationLib& handle);   // defined in AnimationLib.cpp

    // Bumped whenever a model or animation lib is removed or replaced.
    // Handles built from them compare against it to detect staleness.
    uint32_t AssetGeneration();
    void     BumpAssetGeneration();

    // scale -> rotation -> translation (same order the backend uses internally)
    inline Matrix ComposeTransform(const Vector3& pos, const Quaternion& rot, const Vector3& scale) {
        return MatrixMultiply(MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), QuaternionToMatrix(rot)),
                              MatrixTranslate(pos.x, pos.y, pos.z));
    }
}
