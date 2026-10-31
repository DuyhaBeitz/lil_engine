#include "Assets/Sound.hpp"
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

namespace Lil::Assets {

    void SoundData::Load(std::string filename) {
        aliases[0] = LoadSound(filename.c_str());
        for (int i = 1; i < ALIASES_PER_SOUND; i++){
            aliases[i] = LoadSoundAlias(aliases[0]);
        }
    }

    void SoundData::Unload() {
        for (int i = 1; i < ALIASES_PER_SOUND; i++)
            UnloadSoundAlias(aliases[i]);
        UnloadSound(aliases[0]);
    }

    void SoundData::Play(float volume, float pitch) {
        for (int i = 0; i < ALIASES_PER_SOUND; i++){
            if (!IsSoundPlaying(aliases[i])) {
                SetSoundVolume(aliases[i], volume * volume_multiplier * Lil::Audio().GetSFXVolume());
                SetSoundPitch(aliases[i], pitch);
                PlaySound(aliases[i]);
                break;
            }
        }
    }

    void SoundData::Play3D(const Camera &listener, const Vector3 &position, float maxDist, float volume, float pitch) {
        for (int i = 0; i < ALIASES_PER_SOUND; i++){
            if (!IsSoundPlaying(aliases[i])) {
                SetSoundPosition(listener, aliases[i], position, maxDist, volume * volume_multiplier * Lil::Audio().GetSFXVolume());
                SetSoundPitch(aliases[i], pitch);
                PlaySound(aliases[i]);
                break;
            }
        }
    }

    void SoundData::PlayContinuous3D(int alias_index, const Camera &listener, const Vector3 &position, float maxDist, float volume, float pitch) {
        if (alias_index < 0 || alias_index >= ALIASES_PER_SOUND) return;
        SetSoundPosition(listener, aliases[alias_index], position, maxDist, volume * volume_multiplier * Lil::Audio().GetSFXVolume());
        SetSoundPitch(aliases[alias_index], pitch);
        if (!IsSoundPlaying(aliases[alias_index])) {    
            PlaySound(aliases[alias_index]);
        }
    }

    void SoundData::StopContinuous(int alias_index) {
        if (alias_index < 0 || alias_index >= ALIASES_PER_SOUND) return;
        StopSound(aliases[alias_index]);
    }

    // manager
    static std::unordered_map<Sound, SoundData> sounds;
    static std::unordered_map<Sound, SoundSettings> sound_settings;

    std::unordered_map<Sound, SoundSettings> &SoundSettingsMap() {
        return sound_settings;
    }

    static SoundSettings GenSoundSettings(const SoundData& sound_data);
    static void SoundSettingsApply(const SoundSettings& settings, SoundData& sound_data);

    bool SoundExists(Sound handle) {
        return sounds.find(handle) != sounds.end();
    }
    bool SoundSettingsExists(Sound handle) {
        return sound_settings.find(handle) != sound_settings.end();
    }

    SoundData *GetSoundData(Sound handle)
    {
        if (!SoundExists(handle)) return nullptr;
        return &(sounds.at(handle));
    }

    void SoundAdd(Sound handle, SoundData& sound_data) {
        if (SoundExists(handle)) GetSoundData(handle)->Unload();
        sounds[handle] = sound_data;
        if (sound_settings.find(handle) == sound_settings.end()) sound_settings[handle] = GenSoundSettings(sounds.at(handle));
    }

    void SoundAdd(Sound handle, std::string filename) {
        SoundData sound_data;
        sound_data.Load(filename);
        SoundAdd(handle, sound_data);
    }

    void SoundAdd(std::string filename) {
        Sound handle = NameFromPath(filename);
        SoundAdd(handle, filename);
    }

    void SoundRemove(Sound handle) {
        if (SoundExists(handle)) {
            GetSoundData(handle)->Unload();
            sounds.erase(handle);
        }
    }

    SoundSettings GenSoundSettings(const SoundData& sound_data) {
        SoundSettings settings;
        settings.volume_multiplier = sound_data.volume_multiplier;
        return settings;
    }
    void SoundSettingsApply(const SoundSettings& settings, SoundData& sound_data) {
        sound_data.volume_multiplier = settings.volume_multiplier;
    }

    void UpdateAllSoundSettings() {
        for (auto& [handle, settings] : sound_settings) {
            settings = GenSoundSettings(sounds.at(handle));
        }
    }
    void ApplyAllSoundSettings() {
        for (auto& [handle, settings] : sound_settings) {
            SoundSettingsApply(settings, sounds.at(handle));
        }
    }

    void ApplySoundSettings(Sound handle) {
        SoundSettingsApply(sound_settings.at(handle), sounds.at(handle));
    }

    void UnloadAllSounds() {
        for (auto& [handle, sound] : sounds) {
            sound.Unload();
        }
        sounds.clear();
    }

} // namespace Lil::Assets