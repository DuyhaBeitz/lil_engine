#pragma once

#include "CommonIncludes.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"
#include "Sound.hpp"
#include <unordered_map>

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
    field(material_settings)
)
LIL_SER_BEGIN(ModelSettings)
LIL_SER_FIELD(material_settings)
LIL_SER_END()


class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> m_textures;

    std::unordered_map<std::string, R3D_Model> m_models;
    std::unordered_map<std::string, ModelSettings> m_model_settings;
    std::unordered_map<std::string, RenderTexture2D> m_model_previews;

    std::unordered_map<std::string, LilSound> m_sounds;

public:
    void Unload();

    void TextureAdd(std::string key, Texture2D texture);
    void TextureAdd(std::string key, std::string filename);
    void TextureAdd(std::string filename);
    bool TextureExists(std::string key);
    void TextureUnload(std::string key);
    void TextureUnloadAll();

    void ModelAdd(std::string key, R3D_Model model);
    void ModelAdd(std::string key, std::string filename);
    void ModelAdd(std::string filename);
    bool ModelExists(std::string key);
    bool ModelSettingsExists(std::string key);
    void ModelUnload(std::string key);
    void ModelUnloadAll();

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
    ModelSettings* GetModelSettings(std::string key);
    RenderTexture2D* GetModelPreview(std::string key);
    LilSound* GetSound(std::string key);

    std::unordered_map<std::string, Texture2D>& Textures() { return m_textures; }
    std::unordered_map<std::string, R3D_Model>& Models() { return m_models; }
    std::unordered_map<std::string, LilSound>& Sounds() { return m_sounds; }


    template <class Archive>
    void save( Archive & ar ) const {
        std::vector<std::string> texture_keys = {};
        texture_keys.reserve(m_textures.size());
        for (auto& [key, texture] : m_textures) {
            if (IsAssetGenerated(key)) continue;
            texture_keys.push_back(key);
        }

        std::vector<std::string> sound_keys = {};
        sound_keys.reserve(m_sounds.size());
        for (auto& [key, sound] : m_sounds) {
            if (IsAssetGenerated(key)) continue;
            sound_keys.push_back(key);
        }

        ar(texture_keys, sound_keys, m_model_settings);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        Unload();
        std::vector<std::string> texture_keys = {};
        std::vector<std::string> sound_keys = {};

        ar(texture_keys, sound_keys, m_model_settings);
        for (auto& key : texture_keys) TextureAdd("assets/" + key);
        for (auto& key : sound_keys) SoundAdd("assets/" + key);

        for (auto& [key, settings] : m_model_settings) {
            if (IsAssetGenerated(key)) continue;
            ModelAdd("assets/" + key);
            settings.Apply(GetModel(key));
        }
        UpdateModelPreviews();        
    }

};
LIL_DISAMBIGUATE_LOAD_SAVE(ResourceManager)