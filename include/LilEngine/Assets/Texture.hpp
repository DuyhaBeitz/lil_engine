#pragma once

#include <raylib.h>
#include "AssetRef.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"
#include "utils/NameHelper.hpp"

struct TextureSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    int pixel_format = 0;;
    int filter = 0;
    int wrap = 0;
};
LIL_REFLECT(TextureSettings, bases<>,
    field(pixel_format, EnumAttribute{
        EnumAttrItem{"UNCOMPRESSED_GRAYSCALE",     int(PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)},
        EnumAttrItem{"UNCOMPRESSED_GRAY_ALPHA",    int(PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA)},
        EnumAttrItem{"UNCOMPRESSED_R5G6B5",        int(PIXELFORMAT_UNCOMPRESSED_R5G6B5)},
        EnumAttrItem{"UNCOMPRESSED_R8G8B8",        int(PIXELFORMAT_UNCOMPRESSED_R8G8B8)},
        EnumAttrItem{"UNCOMPRESSED_R5G5B5A1",      int(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1)},
        EnumAttrItem{"UNCOMPRESSED_R4G4B4A4",      int(PIXELFORMAT_UNCOMPRESSED_R4G4B4A4)},
        EnumAttrItem{"UNCOMPRESSED_R8G8B8A8",      int(PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)},
        EnumAttrItem{"UNCOMPRESSED_R32",           int(PIXELFORMAT_UNCOMPRESSED_R32)},
        EnumAttrItem{"UNCOMPRESSED_R32G32B32",     int(PIXELFORMAT_UNCOMPRESSED_R32G32B32)},
        EnumAttrItem{"UNCOMPRESSED_R32G32B32A32",  int(PIXELFORMAT_UNCOMPRESSED_R32G32B32A32)},
        EnumAttrItem{"UNCOMPRESSED_R16",           int(PIXELFORMAT_UNCOMPRESSED_R16)},
        EnumAttrItem{"UNCOMPRESSED_R16G16B16",     int(PIXELFORMAT_UNCOMPRESSED_R16G16B16)},
        EnumAttrItem{"UNCOMPRESSED_R16G16B16A16",  int(PIXELFORMAT_UNCOMPRESSED_R16G16B16A16)},
        EnumAttrItem{"COMPRESSED_DXT1_RGB",        int(PIXELFORMAT_COMPRESSED_DXT1_RGB)},
        EnumAttrItem{"COMPRESSED_DXT1_RGBA",       int(PIXELFORMAT_COMPRESSED_DXT1_RGBA)},
        EnumAttrItem{"COMPRESSED_DXT3_RGBA",       int(PIXELFORMAT_COMPRESSED_DXT3_RGBA)},
        EnumAttrItem{"COMPRESSED_DXT5_RGBA",       int(PIXELFORMAT_COMPRESSED_DXT5_RGBA)},
        EnumAttrItem{"COMPRESSED_ETC1_RGB",        int(PIXELFORMAT_COMPRESSED_ETC1_RGB)},
        EnumAttrItem{"COMPRESSED_ETC2_RGB",        int(PIXELFORMAT_COMPRESSED_ETC2_RGB)},
        EnumAttrItem{"COMPRESSED_ETC2_EAC_RGBA",   int(PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA)},
        EnumAttrItem{"COMPRESSED_PVRT_RGB",        int(PIXELFORMAT_COMPRESSED_PVRT_RGB)},
        EnumAttrItem{"COMPRESSED_PVRT_RGBA",       int(PIXELFORMAT_COMPRESSED_PVRT_RGBA)},
        EnumAttrItem{"COMPRESSED_ASTC_4x4_RGBA",   int(PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA)},
        EnumAttrItem{"COMPRESSED_ASTC_8x8_RGBA",   int(PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA)}
    }),
    field(filter, EnumAttribute{
        EnumAttrItem{"NEAREST", int(TEXTURE_FILTER_POINT)},
        EnumAttrItem{"BILINEAR", int(TEXTURE_FILTER_BILINEAR)},
        EnumAttrItem{"TRILINEAR", int(TEXTURE_FILTER_TRILINEAR)},
        EnumAttrItem{"ANISOTROPIC_4X", int(TEXTURE_FILTER_ANISOTROPIC_4X)},
        EnumAttrItem{"ANISOTROPIC_8X", int(TEXTURE_FILTER_ANISOTROPIC_8X)},
        EnumAttrItem{"ANISOTROPIC_16X", int(TEXTURE_FILTER_ANISOTROPIC_16X)}
    }),
    field(wrap, EnumAttribute{
        EnumAttrItem{"TEXTURE_WRAP_REPEAT", int(TEXTURE_WRAP_REPEAT)},
        EnumAttrItem{"TEXTURE_WRAP_CLAMP", int(TEXTURE_WRAP_CLAMP)},
        EnumAttrItem{"TEXTURE_WRAP_MIRROR_REPEAT", int(TEXTURE_WRAP_MIRROR_REPEAT)},
        EnumAttrItem{"TEXTURE_WRAP_MIRROR_CLAMP", int(TEXTURE_WRAP_MIRROR_CLAMP)}
    })    
)
LIL_SER_BEGIN(TextureSettings)
LIL_SER_FIELD(pixel_format)
LIL_SER_FIELD(filter)
LIL_SER_FIELD(wrap)
LIL_SER_END()

namespace Lil::Assets {

    // manager
    class Texture : public AssetRef, public Reflectable {
    public:
        LIL_REFLECTABLE()
        using AssetRef::AssetRef;
    };
}// namespace Lil::Assets

LIL_REFLECT_EX(Lil::Assets::Texture, bases<>, Lil_Assets_Texture)
LIL_ASSET_REF_HASH(Lil::Assets::Texture)

namespace Lil::Assets {
    std::unordered_map<Texture, TextureSettings>& TextureSettingsMap();


    Texture2D* GetTextureData(Texture handle);
    Texture HandleFromTextureData(Texture2D texture_data);
    bool TextureExists(Texture handle);
    bool TextureSettingsExists(Texture handle);
    void TextureAdd(Texture handle, Texture2D& texture_data);
    void TextureAdd(Texture handle, std::string filename);
    void TextureAdd(std::string filename);
    void TextureRemove(Texture handle);

    void UpdateAllTextureSettings();
    void ApplyTextureSettings(Texture handle);
    void ApplyAllTextureSettings();

    template <class Archive>
    void save_textures( Archive & ar ) {
        auto& texture_settings = TextureSettingsMap();
        LIL_SER_FIELD(texture_settings);
    }
        
    template <class Archive>
    void load_textures( Archive & ar ) {
        auto& texture_settings = TextureSettingsMap();
        LIL_SER_FIELD(texture_settings);
        for (auto& [handle, settings] : TextureSettingsMap()) {
            if (IsAssetGenerated(handle.Key())) continue;
            TextureAdd(std::string("assets/") + handle.Key());
        }
        ApplyAllTextureSettings();
    }

    void UnloadAllTextures();

} // namespace Lil::Assets