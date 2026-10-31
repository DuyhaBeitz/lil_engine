#pragma once

#include "Assets/Texture.hpp"
#include "Assets/Model.hpp"
#include "Assets/Sound.hpp"
#include "Assets/AnimationLib.hpp"

namespace Lil::Assets {
    template <class Archive>
    void save_assets( Archive & ar ) {
        save_sounds(ar);
        save_textures(ar);
        save_models(ar);
        save_animation_libs(ar);
    }
        
    template <class Archive>
    void load_assets( Archive & ar ) {
        load_sounds(ar);
        load_textures(ar);
        load_models(ar);
        load_animation_libs(ar);
    }

    inline void Unload() {
        UnloadAllSounds();
        UnloadAllTextures();
        UnloadAllModels();
        UnloadAllAnimationLibs();
    }
} // namespace Lil::Assets