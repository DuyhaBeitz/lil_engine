#include "Sound.hpp"
#include "LilEngine.hpp"

void SetSoundPosition(const Camera& listener, Sound sound, const Vector3& position, float maxDist, float volume_multiplier) {
    Vector3 direction = Vector3Subtract(position, listener.position);
    float distance = Vector3Length(direction);
    
    float dist = 1.0f + (distance / maxDist);
    float attenuation = 1.0f / (dist * dist);
    attenuation = Clamp(attenuation, 0.0f, 1.0f);
    
    Vector3 normalizedDirection = Vector3Normalize(direction);
    Vector3 forward = Vector3Normalize(Vector3Subtract(listener.target, listener.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, listener.up));
    
    float dotProduct = Vector3DotProduct(forward, normalizedDirection);
    if (dotProduct < 0.0f) attenuation *= (1.0f + dotProduct * 0.5f);
    
    float pan = 0.5f - 0.5f * Vector3DotProduct(normalizedDirection, right);

    SetSoundVolume(sound, attenuation*volume_multiplier);
    SetSoundPan(sound, pan);
};

void LilSound::Play() {
    for (int i = 0; i < ALIASES_PER_SOUND; i++){
        if (!IsSoundPlaying(aliases[i])) {
            SetSoundVolume(aliases[i], volume_multiplier * Lil::Audio().GetSFXVolume());
            PlaySound(aliases[i]);
            break;
        }
    }
}

void LilSound::Play3D(const Camera &listener, const Vector3 &position, float maxDist) {
    for (int i = 0; i < ALIASES_PER_SOUND; i++){
        if (!IsSoundPlaying(aliases[i])) {
            SetSoundPosition(listener, aliases[i], position, maxDist, volume_multiplier * Lil::Audio().GetSFXVolume());
            PlaySound(aliases[i]);
            break;
        }
    }
}

void LilSound::PlayContinuous3D(int alias_index, const Camera &listener, const Vector3 &position, float maxDist) {
    if (alias_index < 0 || alias_index >= ALIASES_PER_SOUND) return;
    if (!IsSoundPlaying(aliases[alias_index])) {
        SetSoundPosition(listener, aliases[alias_index], position, maxDist, volume_multiplier * Lil::Audio().GetSFXVolume());
        PlaySound(aliases[alias_index]);
    }
}