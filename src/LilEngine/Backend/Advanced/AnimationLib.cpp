#include "Assets/AnimationLib.hpp"
#include <unordered_map>
#include <vector>
#include <r3d/r3d.h>
#include "utils/NameHelper.hpp"
#include "R3DAccess.hpp"

namespace Lil::Assets {
    static std::unordered_map<std::string, R3D_AnimationLib> s_libs;

    std::vector<AnimationLib> AnimationLibKeys() {
        std::vector<AnimationLib> v;
        v.reserve(s_libs.size());
        for (auto& [key, lib] : s_libs) {
            v.push_back(AnimationLib(key));
        }
        return v;
    }

    bool AnimationLibExists(const AnimationLib& h) { return s_libs.contains(h.Key()); }

    void AnimationLibAdd(const AnimationLib& h, const std::string& filename) {
        R3D_AnimationLib lib = R3D_LoadAnimationLib(filename.c_str());
        //if (!R3D_IsAnimationLibValid(lib)) return;
        AnimationLibRemove(h);                               // replace, freeing the old one
        s_libs.emplace(h.Key(), lib);
        Backend::BumpAssetGeneration();
    }

    void AnimationLibAdd(const std::string& filename) {
        AnimationLibAdd(AnimationLib(NameFromPath(filename)), filename);
    }

    void AnimationLibRemove(const AnimationLib& h) {
        auto it = s_libs.find(h.Key());
        if (it == s_libs.end()) return;
        R3D_UnloadAnimationLib(it->second);
        s_libs.erase(it);
        Backend::BumpAssetGeneration();
    }

    void UnloadAllAnimationLibs() {
        for (auto& [_, lib] : s_libs) R3D_UnloadAnimationLib(lib);
        s_libs.clear();
        Backend::BumpAssetGeneration();
    }

    template <class Archive>
    void save_animation_libs(Archive& ar) {
        std::vector<std::string> animation_lib_keys;
        for (auto& [key, _] : s_libs)
            if (!IsAssetGenerated(key)) animation_lib_keys.push_back(key);
        LIL_SER_FIELD(animation_lib_keys);
    }

    template <class Archive>
    void load_animation_libs(Archive& ar) {
        std::vector<std::string> animation_lib_keys;
        LIL_SER_FIELD(animation_lib_keys);
        for (auto& key : animation_lib_keys)
            AnimationLibAdd(AnimationLib(key), "assets/" + key);
    }

    template void save_animation_libs<ArchiveOut>(ArchiveOut&);
    template void load_animation_libs<ArchiveIn>(ArchiveIn&);
}

namespace Lil::Backend {
    R3D_AnimationLib* R3DAnimationLib(const Assets::AnimationLib& h) {
        auto it = Assets::s_libs.find(h.Key());
        return it == Assets::s_libs.end() ? nullptr : &it->second;
    }
}