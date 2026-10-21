#pragma once

#include "CommonIncludes.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"
#include "Sound.hpp"
#include <unordered_map>
#include "utils/NameHelper.hpp"

struct AlbedoMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    std::string texture_key = "None";
    Color color;
};
LIL_REFLECT(AlbedoMap, bases<>,
    field(texture_key, TextureKeyAttribute{}),
    field(color)
)
LIL_SER_BEGIN(AlbedoMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(color)
LIL_SER_END()

struct NormalMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    std::string texture_key = "None";
    float scale;
};
LIL_REFLECT(NormalMap, bases<>,
    field(texture_key, TextureKeyAttribute{}),
    field(scale)
)
LIL_SER_BEGIN(NormalMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(scale)
LIL_SER_END()

struct EmissionMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    std::string texture_key = "None";
    float energy;
};
LIL_REFLECT(EmissionMap, bases<>,
    field(texture_key, TextureKeyAttribute{}),
    field(energy)
)
LIL_SER_BEGIN(EmissionMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(energy)
LIL_SER_END()

struct OrmMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    std::string texture_key = "None";
    float occlusion;
    float roughness;
    float metalness;
    float specular;
};
LIL_REFLECT(OrmMap, bases<>,
    field(texture_key, TextureKeyAttribute{}),
    field(occlusion),
    field(roughness),
    field(metalness),
    field(specular)
)
LIL_SER_BEGIN(OrmMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(occlusion)
LIL_SER_FIELD(roughness)
LIL_SER_FIELD(metalness)
LIL_SER_FIELD(specular)
LIL_SER_END()

struct MaterialSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    
    static MaterialSettings GenerateFrom(const R3D_Material& material);
    void Apply(R3D_Material* material) const;

    Vector2 uvScale;
    Vector2 uvOffset;

    AlbedoMap albedo;
    NormalMap normal;
    EmissionMap emission;
    OrmMap orm;
};
LIL_REFLECT(MaterialSettings, bases<>,
    field(uvScale),
    field(uvOffset),
    field(albedo),
    field(normal),
    field(emission),
    field(orm)
)
LIL_SER_BEGIN(MaterialSettings)
LIL_SER_FIELD(uvScale)
LIL_SER_FIELD(uvOffset)
LIL_SER_FIELD(albedo)
LIL_SER_FIELD(normal)
LIL_SER_FIELD(emission)
LIL_SER_FIELD(orm)
LIL_SER_END()

struct ModelSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    std::list<MaterialSettings> material_settings;

    static ModelSettings GenerateFrom(const R3D_Model& model);
    void Apply(R3D_Model* model) const;
};
LIL_REFLECT_EX(std::list<MaterialSettings>, bases<>, std_list_MaterialSettings)
LIL_REFLECT(ModelSettings, bases<>,
    field(material_settings, ContainerNoAddAttribute{}, ContainerNoEraseAttribute{})
)
LIL_SER_BEGIN(ModelSettings)
LIL_SER_FIELD(material_settings)
LIL_SER_END()

struct TextureSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    int pixel_format = 0;;
    int filter = 0;
    int wrap = 0;

    static TextureSettings GenerateFrom(const Texture2D& texture);
    void Apply(Texture2D* texture) const;
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

class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> m_textures;

    std::unordered_map<std::string, R3D_Model> m_models;
    std::unordered_map<std::string, R3D_AnimationLib> m_animation_libs;
    std::unordered_map<std::string, ModelSettings> m_model_settings;
    std::unordered_map<std::string, TextureSettings> m_texture_settings;
    std::unordered_map<std::string, RenderTexture2D> m_model_previews;

    std::unordered_map<std::string, LilSound> m_sounds;

public:
    void Unload();

    void TextureAdd(std::string key, Texture2D texture);
    void TextureAdd(std::string key, std::string filename);
    void TextureAdd(std::string filename);
    bool TextureExists(std::string key);
    bool TextureSettingsExists(std::string key);
    void TextureUnload(std::string key);
    void TextureUnloadAll();

    void ModelAdd(std::string key, R3D_Model model);
    void ModelAdd(std::string key, std::string filename);
    void ModelAdd(std::string filename);
    bool ModelExists(std::string key);
    bool ModelSettingsExists(std::string key);
    void ModelUnload(std::string key);
    void ModelUnloadAll();

    bool AnimationLibExists(std::string key);
    void AnimationLibAdd(std::string key, R3D_AnimationLib animation_lib);
    void AnimationLibAdd(std::string key, std::string filename);
    void AnimationLibAdd(std::string filename);

    void ApplyTextureSettings();
    void ApplyModelSettings();

    bool ModelPreviewExists(std::string key);
    void UnloadModelPreviews();
    void UpdateModelPreviews();

    void SoundAdd(std::string key, LilSound sound);
    void SoundAdd(std::string key, std::string filename);
    void SoundAdd(std::string filename);
    bool SoundExists(std::string key);
    void SoundUnload(std::string key);
    void SoundUnloadAll();

    std::string KeyFromTexture(Texture2D texture);
    Texture2D* GetTexture(std::string key);
    R3D_Model* GetModel(std::string key);
    R3D_AnimationLib* GetAnimationLib(std::string key);
    TextureSettings* GetTextureSettings(std::string key);
    ModelSettings* GetModelSettings(std::string key);
    RenderTexture2D* GetModelPreview(std::string key);
    LilSound* GetSound(std::string key);

    std::unordered_map<std::string, Texture2D>& Textures() { return m_textures; }
    std::unordered_map<std::string, R3D_Model>& Models() { return m_models; }
    std::unordered_map<std::string, LilSound>& Sounds() { return m_sounds; }


    template <class Archive>
    void save( Archive & ar ) const {
        std::vector<std::string> sound_keys = {};
        sound_keys.reserve(m_sounds.size());
        for (auto& [key, sound] : m_sounds) {
            if (IsAssetGenerated(key)) continue;
            sound_keys.push_back(key);
        }

        std::vector<std::string> animation_lib_keys = {};
        animation_lib_keys.reserve(m_animation_libs.size());
        for (auto& [key, animation_lib] : m_animation_libs) {
            if (IsAssetGenerated(key)) continue;
            animation_lib_keys.push_back(key);
        }

        LIL_SER_FIELD(sound_keys);
        LIL_SER_FIELD(m_texture_settings);
        LIL_SER_FIELD(m_model_settings);
        LIL_SER_FIELD(animation_lib_keys);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        Unload();
        std::vector<std::string> sound_keys = {};
        std::vector<std::string> animation_lib_keys = {};

        LIL_SER_FIELD(sound_keys);
        LIL_SER_FIELD(m_texture_settings);
        LIL_SER_FIELD(m_model_settings);
        LIL_SER_FIELD(animation_lib_keys);

        for (auto& [key, settings] : m_texture_settings) {
            if (IsAssetGenerated(key)) continue;
            TextureAdd("assets/" + key);
        }
        for (auto& [key, settings] : m_model_settings) {
            if (IsAssetGenerated(key)) continue;
            ModelAdd("assets/" + key);
        }

        for (auto& key : sound_keys) SoundAdd("assets/" + key);
        for (auto& key : animation_lib_keys) AnimationLibAdd("assets/" + key);

        ApplyTextureSettings();
        ApplyModelSettings();
        UpdateModelPreviews();        
    }

};
LIL_DISAMBIGUATE_LOAD_SAVE(ResourceManager)