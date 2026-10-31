#include "Assets/Texture.hpp"
#include <raylib.h>

namespace Lil::Assets {
    // manager
    static std::unordered_map<Texture, Texture2D> textures;
    static std::unordered_map<Texture, TextureSettings> texture_settings;

    std::unordered_map<Texture, TextureSettings>& TextureSettingsMap() {
        return texture_settings;
    }

    static TextureSettings GenTextureSettings(const Texture2D& texture_data);
    static void TextureSettingsApply(const TextureSettings& settings, Texture2D& texture_data);

    bool TextureExists(Texture handle) {
        return textures.find(handle) != textures.end();
    }
    bool TextureSettingsExists(Texture handle) {
        return texture_settings.find(handle) != texture_settings.end();
    }

    Texture2D *GetTextureData(Texture handle) {
        if (!TextureExists(handle)) return nullptr;
        return &(textures.at(handle));
    }

    Texture HandleFromTextureData(Texture2D texture_data) {
        std::string res = "None";
        for (auto& [handle, t] : textures) {
            if (t.id != 0 && t.id == texture_data.id) {
                res = handle.Key();
                break;
            }
        }
        return Texture(res);
    }

    void TextureAdd(Texture handle, Texture2D& texture_data) {
        if (TextureExists(handle)) UnloadTexture(texture_data);
        textures[handle] = texture_data;
        if (texture_settings.find(handle) == texture_settings.end()) texture_settings[handle] = GenTextureSettings(textures.at(handle));
    }

    void TextureAdd(Texture handle, std::string filename) {
        Texture2D texture_data = LoadTexture(filename.c_str());
        TextureAdd(handle, texture_data);
    }

    void TextureAdd(std::string filename) {
        Texture handle = NameFromPath(filename);
        TextureAdd(handle, filename);
    }

    void TextureRemove(Texture handle) {
        if (TextureExists(handle)) {
            ::UnloadTexture(*GetTextureData(handle));
            textures.erase(handle);
        }
    }

    TextureSettings GenTextureSettings(const Texture2D& texture_data) {
        TextureSettings settings;
        settings.pixel_format = texture_data.format;
        settings.filter       = TEXTURE_FILTER_POINT;
        settings.wrap         = TEXTURE_WRAP_CLAMP;
        return settings;
    }
    void TextureSettingsApply(const TextureSettings& settings, Texture2D& texture_data) {
        texture_data.format = settings.pixel_format;
        SetTextureFilter(texture_data, settings.filter);
        SetTextureWrap(texture_data, settings.wrap);
    }

    void UpdateAllTextureSettings() {
        for (auto& [handle, settings] : texture_settings) {
            settings = GenTextureSettings(textures.at(handle));
        }
    }
    void ApplyTextureSettings(Texture handle) {
        TextureSettingsApply(texture_settings.at(handle), textures.at(handle));
    }
    void ApplyAllTextureSettings() {
        for (auto& [handle, settings] : texture_settings) {
            auto it = textures.find(handle);
            if (it == textures.end()) continue;
            TextureSettingsApply(settings, it->second);
        }
    }

    void UnloadAllTextures() {
        for (auto& [handle, texture] : textures) {
            UnloadTexture(texture);
        }
        textures.clear();
    }

} // namespace Lil::Assets

