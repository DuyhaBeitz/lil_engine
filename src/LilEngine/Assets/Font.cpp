#include "Assets/Font.hpp"
#include <raylib.h>

namespace Lil::Assets {
    // manager
    static std::unordered_map<Font, ::Font> fonts;
    static std::unordered_map<Font, Image> images;
    static std::unordered_map<Font, std::string> font_filepaths;
    static std::unordered_map<Font, FontSettings> font_settings;

    std::unordered_map<Font, FontSettings>& FontSettingsMap() {
        return font_settings;
    }

    static FontSettings GenFontSettings(const ::Font& font_data);

    bool FontExists(Font handle) {
        return fonts.find(handle) != fonts.end();
    }
    bool FontSettingsExists(Font handle) {
        return font_settings.find(handle) != font_settings.end();
    }

    ::Font *GetFontData(Font handle) {
        if (!FontExists(handle)) return nullptr;
        return &(fonts.at(handle));
    }

    void FontAdd(Font handle, ::Font& font_data) {
        if (FontExists(handle)) UnloadFont(font_data);
        fonts[handle] = font_data;
        if (font_settings.find(handle) == font_settings.end()) font_settings[handle] = GenFontSettings(fonts.at(handle));
        ApplyFontSettings(handle);
    }

    void FontAdd(Font handle, std::string filename, int fontSize) {
        ::Font font = LoadFontEx(filename.c_str(), fontSize, nullptr, 0);
        font_filepaths[handle] = filename;
        FontAdd(handle, font);
    }

    void FontAdd(std::string filename, int fontSize) {
        Font handle = NameFromPath(filename);
        FontAdd(handle, filename, fontSize);
    }

    void FontRemove(Font handle) {
        if (FontExists(handle)) {
            ::UnloadFont(*GetFontData(handle));
            fonts.erase(handle);
        }
    }

    FontSettings GenFontSettings(const ::Font& font_data) {
        FontSettings settings;
        settings.fontSize = font_data.baseSize;
        return settings;
    }

    void UpdateAllFontSettings() {
        for (auto& [handle, settings] : font_settings) {
            settings = GenFontSettings(fonts.at(handle));
        }
    }
    void ApplyFontSettings(Font handle) {
        if (FontExists(handle)) UnloadFont(fonts[handle]);
        fonts[handle] = LoadFontEx(font_filepaths[handle].c_str(), font_settings[handle].fontSize, nullptr, 0);
    }
    void ApplyAllFontSettings() {
        for (auto& [handle, settings] : font_settings) {
            auto it = fonts.find(handle);
            if (it == fonts.end()) continue;
            ApplyFontSettings(handle);
        }
    }

    void UnloadAllFonts() {
        for (auto& [handle, font] : fonts) {
            UnloadFont(font);
        }
        fonts.clear();
    }

} // namespace Lil::Assets

