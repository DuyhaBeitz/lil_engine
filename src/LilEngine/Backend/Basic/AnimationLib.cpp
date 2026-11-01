#include "Assets/AnimationLib.hpp"

#include <raylib.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "BackendAccess.hpp"

#include "utils/NameHelper.hpp"

namespace Lil::Assets {

    // raylib's ModelAnimation stores no frame rate, so it is captured here
    // when the library is loaded.
    //
    //  * glTF / GLB / M3D: raylib resamples every animation at a fixed step
    //    (GLTF_ANIMDELAY / M3D_ANIMDELAY = 17 ms, ~58.8 fps in raylib 5.5),
    //    regardless of the source file's rate. If you upgrade raylib, check
    //    those defines in rmodels.c and adjust kResampledAnimationFps.
    //  * IQM: frames are the file's raw frames; the rate is per animation
    //    in the file header and is read directly from it.
    //  * anything else: kDefaultAnimationFps.
    static constexpr float kDefaultAnimationFps = 30.0f;
    static constexpr float kResampledAnimationFps = 1000.0f / 17.0f;

    struct AnimationLibrary {
        ModelAnimation* animations = nullptr;
        unsigned int count = 0;
        std::vector<float> fps;   // one entry per animation
    };

    static uint32_t ReadU32LE(const unsigned char* p) {
        return static_cast<uint32_t>(p[0]) |
               (static_cast<uint32_t>(p[1]) << 8) |
               (static_cast<uint32_t>(p[2]) << 16) |
               (static_cast<uint32_t>(p[3]) << 24);
    }

    // Reads iqmanim.framerate for every animation in an .iqm file.
    // Returns an empty vector if the file can't be parsed.
    static std::vector<float> ReadIqmFrameRates(
        const std::string& filename
    ) {
        std::vector<float> rates;

        std::ifstream file(filename, std::ios::binary);

        if (!file) {
            return rates;
        }

        // iqmheader: char magic[16], then uint32 fields. num_anims is
        // field #17 (byte 84) and ofs_anims field #18 (byte 88).
        unsigned char header[92] = {};

        if (!file.read(reinterpret_cast<char*>(header), sizeof(header))) {
            return rates;
        }

        if (std::memcmp(header, "INTERQUAKEMODEL", 16) != 0) {
            return rates;
        }

        const uint32_t numAnims = ReadU32LE(header + 84);
        const uint32_t ofsAnims = ReadU32LE(header + 88);

        if (numAnims == 0 || numAnims > 4096) {
            return rates;
        }

        // iqmanim: name, first_frame, num_frames, framerate(float), flags.
        constexpr size_t kRecordSize = 20;
        constexpr size_t kFramerateOffset = 12;

        std::vector<unsigned char> records(numAnims * kRecordSize);

        file.seekg(static_cast<std::streamoff>(ofsAnims), std::ios::beg);

        if (!file.read(
                reinterpret_cast<char*>(records.data()),
                static_cast<std::streamsize>(records.size()))) {
            return rates;
        }

        rates.reserve(numAnims);

        for (uint32_t i = 0; i < numAnims; ++i) {
            const uint32_t bits = ReadU32LE(
                records.data() + i * kRecordSize + kFramerateOffset
            );

            float fps = 0.0f;
            std::memcpy(&fps, &bits, sizeof(fps));

            rates.push_back(
                (std::isfinite(fps) && fps > 0.0f)
                    ? fps
                    : kDefaultAnimationFps
            );
        }

        return rates;
    }

    static std::vector<float> DetectFrameRates(
        const std::string& filename,
        unsigned int count
    ) {
        std::string ext =
            std::filesystem::path(filename).extension().string();

        std::transform(
            ext.begin(),
            ext.end(),
            ext.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );

        std::vector<float> rates;

        if (ext == ".iqm") {
            rates = ReadIqmFrameRates(filename);
        } else if (ext == ".glb" || ext == ".gltf" || ext == ".m3d") {
            rates.assign(count, kResampledAnimationFps);
        }

        // Unknown format, unparsable file, or count mismatch with what
        // raylib loaded: fall back rather than index out of range.
        if (rates.size() != count) {
            rates.assign(count, kDefaultAnimationFps);
        }

        return rates;
    }

    static std::unordered_map<std::string, AnimationLibrary> s_libs;

    std::vector<AnimationLib> AnimationLibKeys() {
        std::vector<AnimationLib> result;
        result.reserve(s_libs.size());

        for (const auto& [key, lib] : s_libs) {
            (void)lib;
            result.emplace_back(key);
        }

        return result;
    }

    bool AnimationLibExists(const AnimationLib& handle) {
        return s_libs.contains(handle.Key());
    }

    void AnimationLibAdd(
        const AnimationLib& handle,
        const std::string& filename
    ) {
        int animationCount = 0;

        ModelAnimation* animations =
            LoadModelAnimations(filename.c_str(), &animationCount);

        if (!animations || animationCount <= 0) {
            if (animations) {
                UnloadModelAnimations(
                    animations,
                    animationCount
                );
            }

            return;
        }

        // Replace the old library only after the new library loaded
        // successfully, so a failed reload does not destroy the old one.
        // AnimationLibRemove() bumps the asset generation if (and only if)
        // there was an old library to replace. Adding a brand new library
        // must not bump it, otherwise every existing AnimationPlayer goes
        // stale and stops drawing.
        AnimationLibRemove(handle);

        AnimationLibrary library;
        library.animations = animations;
        library.count = static_cast<unsigned int>(animationCount);
        library.fps = DetectFrameRates(filename, library.count);

        TraceLog(
            LOG_INFO,
            "ANIMATION LIB: [%s] %u animation(s), first at %.2f fps",
            filename.c_str(),
            library.count,
            library.fps.empty() ? kDefaultAnimationFps : library.fps[0]
        );

        s_libs.emplace(handle.Key(), library);
    }

    void AnimationLibAdd(const std::string& filename) {
        AnimationLibAdd(
            AnimationLib(NameFromPath(filename)),
            filename
        );
    }

    void AnimationLibRemove(const AnimationLib& handle) {
        auto it = s_libs.find(handle.Key());

        if (it == s_libs.end()) {
            return;
        }

        if (it->second.animations) {
            UnloadModelAnimations(
                it->second.animations,
                static_cast<int>(it->second.count)
            );
        }

        s_libs.erase(it);

        Backend::BumpAssetGeneration();
    }

    void UnloadAllAnimationLibs() {
        for (auto& [key, library] : s_libs) {
            (void)key;

            if (library.animations) {
                UnloadModelAnimations(
                    library.animations,
                    static_cast<int>(library.count)
                );
            }
        }

        s_libs.clear();

        Backend::BumpAssetGeneration();
    }

    template <class Archive>
    void save_animation_libs(Archive& ar) {
        std::vector<std::string> animation_lib_keys;

        for (const auto& [key, library] : s_libs) {
            (void)library;

            if (!IsAssetGenerated(key)) {
                animation_lib_keys.push_back(key);
            }
        }

        LIL_SER_FIELD(animation_lib_keys);
    }

    template <class Archive>
    void load_animation_libs(Archive& ar) {
        std::vector<std::string> animation_lib_keys;

        LIL_SER_FIELD(animation_lib_keys);

        for (const auto& key : animation_lib_keys) {
            AnimationLibAdd(
                AnimationLib(key),
                "assets/" + key
            );
        }
    }

    template void save_animation_libs<ArchiveOut>(ArchiveOut&);
    template void load_animation_libs<ArchiveIn>(ArchiveIn&);

} // namespace Lil::Assets

namespace Lil::Backend {
    ::ModelAnimation* RaylibAnimationLib(
        const Assets::AnimationLib& handle,
        unsigned int* animationCount
    ) {
        auto it = Assets::s_libs.find(handle.Key());

        if (it == Assets::s_libs.end()) {
            if (animationCount) {
                *animationCount = 0;
            }

            return nullptr;
        }

        if (animationCount) {
            *animationCount = it->second.count;
        }

        return it->second.animations;
    }

    float RaylibAnimationFps(
        const Assets::AnimationLib& handle,
        unsigned int animationIndex
    ) {
        auto it = Assets::s_libs.find(handle.Key());

        if (it == Assets::s_libs.end() ||
            animationIndex >= it->second.fps.size()) {
            return Assets::kDefaultAnimationFps;
        }

        const float fps = it->second.fps[animationIndex];

        return (std::isfinite(fps) && fps > 0.0f)
            ? fps
            : Assets::kDefaultAnimationFps;
    }

} // namespace Lil::Backend