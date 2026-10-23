#pragma once

#include "Assets/Texture.hpp"
#include "Assets/Model.hpp"
#include "Assets/Sound.hpp"

namespace Lil::Assets {
    template <class Archive>
    void save_assets( Archive & ar ) {
        save_sounds(ar);
        save_textures(ar);
        save_models(ar);
    }
        
    template <class Archive>
    void load_assets( Archive & ar ) {
        load_sounds(ar);
        load_textures(ar);
        load_models(ar);
    }

    void Unload() {
        UnloadAllSounds();
        UnloadAllTextures();
        UnloadAllModels();
    }
} // namespace Lil::Assets