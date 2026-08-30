
#pragma once

#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <string>

void SetSoundPosition(const Camera& listener, Sound sound, const Vector3& position, float maxDist, float volume_multiplier = 1.0f);

#define ALIASES_PER_SOUND 32

struct LilSound {
    Sound aliases[ALIASES_PER_SOUND];
    float volume_multiplier = 1.0f;

    void Load(std::string filename);
    void Unload();

    void Play();
    void Play3D(const Camera& listener, const Vector3& position, float maxDist);
    void PlayContinuous3D(int alias_index, const Camera& listener, const Vector3& position, float maxDist);
    void StopContinuous(int alias_index);
};