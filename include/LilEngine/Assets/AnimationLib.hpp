#pragma once
#include <string>
#include "AssetRef.hpp"
#include "Reflection.hpp"
#include "Serialization.hpp"

namespace Lil::Assets {
    class AnimationLib : public AssetRef, public Reflectable {
    public:
        LIL_REFLECTABLE()
        using AssetRef::AssetRef;
        AnimationLib() = default;
    };
}
LIL_REFLECT_EX(Lil::Assets::AnimationLib, bases<>, Lil_Assets_AnimationLib)

namespace Lil::Assets {
    std::vector<AnimationLib> AnimationLibKeys();

    bool AnimationLibExists(const AnimationLib& handle);
    void AnimationLibAdd(const AnimationLib& handle, const std::string& filename);
    void AnimationLibAdd(const std::string& filename);
    void AnimationLibRemove(const AnimationLib& handle);
    void UnloadAllAnimationLibs();

    template <class Archive> void save_animation_libs(Archive& ar);
    template <class Archive> void load_animation_libs(Archive& ar);
}