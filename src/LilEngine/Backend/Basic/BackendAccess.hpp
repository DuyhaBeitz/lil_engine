#pragma once
#include <cstdint>
#include <r3d/r3d.h>
#include "Assets/Model.hpp"
#include "Assets/AnimationLib.hpp"

namespace Lil::Backend {
    ::Model* GetRaylibModel(const Assets::Model& handle);
    ::ModelAnimation* RaylibAnimationLib(const Assets::AnimationLib& handle, unsigned int* animationCount);

    // Playback rate (frames per second) of one animation in a library.
    // raylib's ModelAnimation has no frame-rate field, so it is recorded
    // when the library is loaded. Always returns a finite value > 0.
    float RaylibAnimationFps(const Assets::AnimationLib& handle, unsigned int animationIndex);

    // Bumped whenever a model or animation lib is removed or replaced.
    // Handles built from them compare against it to detect staleness.
    uint32_t AssetGeneration();
    void     BumpAssetGeneration();
}