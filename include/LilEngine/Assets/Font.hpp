#pragma once

#include <raylib.h>
#include "AssetRef.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"
#include "utils/NameHelper.hpp"

struct FontSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    int fontSize;
};
LIL_REFLECT(FontSettings, bases<>,
    field(fontSize)
)
LIL_SER_BEGIN(FontSettings)
LIL_SER_FIELD(fontSize)
LIL_SER_END()

namespace Lil::Assets {

    // manager
    class Font : public AssetRef, public Reflectable {
    public:
        LIL_REFLECTABLE()
        using AssetRef::AssetRef;
    };
}// namespace Lil::Assets

LIL_REFLECT_EX(Lil::Assets::Font, bases<>, Lil_Assets_Font)
LIL_ASSET_REF_HASH(Lil::Assets::Font)

namespace Lil::Assets {
    std::unordered_map<Font, FontSettings>& FontSettingsMap();

    constexpr int default_font_size = 64;

    ::Font* GetFontData(Font handle);
    bool FontExists(Font handle);
    bool FontSettingsExists(Font handle);
    void FontAdd(Font handle, ::Font& font_data);
    void FontAdd(Font handle, std::string filename, int fontSize);
    void FontAdd(std::string filename, int fontSize);
    void FontRemove(Font handle);

    void UpdateAllFontSettings();
    void ApplyFontSettings(Font handle);
    void ApplyAllFontSettings();

    template <class Archive>
    void save_fonts( Archive & ar ) {
        auto& font_settings = FontSettingsMap();
        LIL_SER_FIELD(font_settings);
    }
        
    template <class Archive>
    void load_fonts( Archive & ar ) {
        auto& font_settings = FontSettingsMap();
        LIL_SER_FIELD(font_settings);
        for (auto& [handle, settings] : FontSettingsMap()) {
            if (IsAssetGenerated(handle.Key())) continue;
            FontAdd(std::string("assets/") + handle.Key(), default_font_size);
        }
        ApplyAllFontSettings();
    }

    void UnloadAllFonts();

} // namespace Lil::Assets