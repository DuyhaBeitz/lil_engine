
#pragma once

#include <raylib.h>
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "utils/NameHelper.hpp"
#include "AssetRef.hpp"
#include <unordered_map>

void SetSoundPosition(const Camera& listener, Sound sound, const Vector3& position, float maxDist, float volume_multiplier = 1.0f);

#define ALIASES_PER_SOUND 32

struct SoundSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    float volume_multiplier = 1.0f;
};
LIL_REFLECT(SoundSettings, bases<>, field(volume_multiplier))
LIL_SER_BEGIN(SoundSettings)
LIL_SER_FIELD(volume_multiplier)
LIL_SER_END()

namespace Lil::Assets {
    struct SoundData {
        ::Sound aliases[ALIASES_PER_SOUND];
        float volume_multiplier = 1.0f;

        void Load(std::string filename);
        void Unload();

        void Play(float volume = 1.0f, float pitch = 1.0f);
        void Play3D(const Camera& listener, const Vector3& position, float maxDist, float volume = 1.0f, float pitch = 1.0f);
        void PlayContinuous3D(int alias_index, const Camera& listener, const Vector3& position, float maxDist, float volume = 1.0f, float pitch = 1.0f);
        void StopContinuous(int alias_index);
    };

    // manager
    class Sound : public AssetRef, public Reflectable {
    public:
        LIL_REFLECTABLE()
        using AssetRef::AssetRef;
    };
} // namespace Lil::Assets

LIL_REFLECT_EX(Lil::Assets::Sound, bases<>, Lil_Assets_Sound)
LIL_ASSET_REF_HASH(Lil::Assets::Sound)

namespace Lil::Assets {
    std::unordered_map<Sound, SoundSettings>& SoundSettingsMap();

    SoundData* GetSoundData(Sound handle);
    bool SoundExists(Sound handle);
    bool SoundSettingsExists(Sound handle);
    void SoundAdd(Sound handle, SoundData& sound_data);
    void SoundAdd(Sound handle, std::string filename);
    void SoundAdd(std::string filename);
    void SoundRemove(Sound handle);

    void UpdateAllSoundSettings();
    void ApplyAllSoundSettings();
    void ApplySoundSettings(Sound handle);

    template <class Archive>
    void save_sounds( Archive & ar ) {
        auto& sound_settings = SoundSettingsMap();
        LIL_SER_FIELD(sound_settings);
    }
        
    template <class Archive>
    void load_sounds( Archive & ar ) {
        auto& sound_settings = SoundSettingsMap();
        LIL_SER_FIELD(sound_settings);
        for (auto& [handle, settings] : SoundSettingsMap()) {
            if (IsAssetGenerated(handle.Key())) continue;
            SoundAdd(std::string("assets/") + handle.Key());
        }
        ApplyAllSoundSettings();
    }

    void UnloadAllSounds();
} // namespace Lil::Assets