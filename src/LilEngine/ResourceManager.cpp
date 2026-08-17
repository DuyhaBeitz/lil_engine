#include "ResourceManager.hpp"
#include "LilEngine.hpp"
#include <set>

Camera3D model_preview_camera = {
    .position = (Vector3){ 10.0f,10.0f, 10.0f },  // Camera position
    .target = (Vector3){ 0.0f, 0.0f, 0.0f },      // Camera looking at point
    .up = (Vector3){ 0.0f, 1.0f, 0.0f },          // Camera up vector (rotation towards target)
    .fovy = 60.0f,                                // Camera field-of-view Y
    .projection = CAMERA_PERSPECTIVE              // Camera mode type
};

MaterialSettings MaterialSettings::GenerateFrom(const R3D_Material &material) {
    MaterialSettings settings;
    settings.uvOffset = material.uvOffset;
    settings.uvScale = material.uvScale;

    settings.albedo.texture_key = Lil::Resources().KeyFromTexture(material.albedo.texture);
    settings.albedo.color = material.albedo.color;

    settings.normal.texture_key = Lil::Resources().KeyFromTexture(material.normal.texture);
    settings.normal.scale = material.normal.scale;

    settings.emission.texture_key = Lil::Resources().KeyFromTexture(material.emission.texture);
    settings.emission.energy = material.emission.energy;

    settings.orm.texture_key = Lil::Resources().KeyFromTexture(material.orm.texture);
    settings.orm.occlusion = material.orm.occlusion;
    settings.orm.roughness = material.orm.roughness;
    settings.orm.metalness = material.orm.metalness;
    settings.orm.specular = material.orm.specular;

    return settings;
}

void MaterialSettings::Apply(R3D_Material *material) const {
    material->uvOffset = uvOffset;
    material->uvScale = uvScale;

    if (Texture2D* t = Lil::Resources().GetTexture(albedo.texture_key)) material->albedo.texture = *t;
    material->albedo.color = albedo.color;

    if (Texture2D* t = Lil::Resources().GetTexture(normal.texture_key)) material->normal.texture = *t;
    material->normal.scale = normal.scale;

    if (Texture2D* t = Lil::Resources().GetTexture(emission.texture_key)) material->emission.texture = *t;
    material->emission.energy = emission.energy;

    if (Texture2D* t = Lil::Resources().GetTexture(orm.texture_key)) material->orm.texture = *t;
    material->orm.occlusion = orm.occlusion;
    material->orm.roughness = orm.roughness;
    material->orm.metalness = orm.metalness;
    material->orm.specular = orm.specular;
}

ModelSettings ModelSettings::GenerateFrom(const R3D_Model &model) {
    ModelSettings settings;

    for (int i = 0; i < model.materialCount; i++) {
        settings.material_settings.emplace_back(MaterialSettings::GenerateFrom(model.materials[i]));
    }    
    return settings;
}

void ModelSettings::Apply(R3D_Model *model) const {
    assert(model->materialCount == material_settings.size() && "materialCount and material_settings.size() don't match");
    for (int i = 0; i < model->materialCount; i++) {
        auto it = std::next(material_settings.begin(), i);
        it->Apply(&(model->materials[i]));
    }
}

void ResourceManager::Unload() {
    UnloadModelPreviews();
    ModelUnloadAll();
    TextureUnloadAll();
}

void ResourceManager::TextureAdd(std::string key, Texture2D texture) {
    if (TextureExists(key)) UnloadTexture(m_textures[key]);
    m_textures[key] = texture;
}

void ResourceManager::TextureAdd(std::string key, std::string filename) {
    Texture2D texture = LoadTexture(filename.c_str());
    TextureAdd(key, texture);
}

void ResourceManager::TextureAdd(std::string filename) {
    std::string key = NameFromPath(filename);
    TextureAdd(key, filename);
}

bool ResourceManager::TextureExists(std::string key) {
    return m_textures.find(key) != m_textures.end();
}

void ResourceManager::TextureUnload(std::string key) {
    if (TextureExists(key)) {
        UnloadTexture(*GetTexture(key));
        m_models.erase(key);
    }
}

void ResourceManager::TextureUnloadAll() {
    for (auto& [key, texture] : m_textures) UnloadTexture(texture);
    m_textures.clear();
}

void ResourceManager::ModelAdd(std::string key, R3D_Model model) {
    if (ModelExists(key)) R3D_UnloadModel(m_models[key], true);
    m_models[key] = model;
    if (m_model_settings.find(key) == m_model_settings.end()) m_model_settings[key] = ModelSettings::GenerateFrom(m_models.at(key));
}

void ResourceManager::ModelAdd(std::string key, std::string filename) {
    R3D_Model model = R3D_LoadModelEx(filename.c_str(), R3D_IMPORT_RETAIN_MESH_DATA);
    ModelAdd(key, model);

    for (int i = 0; i < model.materialCount; i++) {
        R3D_Material& mat = model.materials[i];
        if (mat.albedo.texture.id != 0)   TextureAdd("Gen_albedo_"   + key, mat.albedo.texture);
        if (mat.normal.texture.id != 0)   TextureAdd("Gen_normal_"   + key, mat.normal.texture);
        if (mat.emission.texture.id != 0) TextureAdd("Gen_emission_" + key, mat.emission.texture);
        if (mat.orm.texture.id != 0)      TextureAdd("Gen_orm_"      + key, mat.orm.texture);
    }
}

void ResourceManager::ModelAdd(std::string filename) {
    std::string key = NameFromPath(filename);
    ModelAdd(key, filename);
}

bool ResourceManager::ModelExists(std::string key) {
    return m_models.find(key) != m_models.end();
}

bool ResourceManager::ModelSettingsExists(std::string key) {
    return m_model_settings.find(key) != m_model_settings.end();
}

void ResourceManager::ModelUnload(std::string key) {
    if (ModelExists(key)) {
        R3D_UnloadModel(*GetModel(key), true);
        m_models.erase(key);
    }
}

void ResourceManager::ModelUnloadAll() {
    for (auto& [key, model] : m_models) {
        R3D_UnloadModel(model, true);
    }
    m_models.clear();
}

bool ResourceManager::AnimationLibExists(std::string key) {
    return m_animation_libs.find(key) != m_animation_libs.end();
}

void ResourceManager::AnimationLibAdd(std::string key, R3D_AnimationLib animation_lib) {
    if (AnimationLibExists(key)) R3D_UnloadAnimationLib(m_animation_libs[key]);
    m_animation_libs[key] = animation_lib;
}

void ResourceManager::AnimationLibAdd(std::string key, std::string filename) {
    R3D_AnimationLib animation_lib = R3D_LoadAnimationLib(filename.c_str());
    AnimationLibAdd(key, animation_lib);
}

void ResourceManager::AnimationLibAdd(std::string filename) {
    std::string key = NameFromPath(filename);
    AnimationLibAdd(key, filename);
}

void ResourceManager::ApplyModelSettings() {
    for (auto& [key, model] : m_models) {
        m_model_settings.at(key).Apply(&model);
    }
}

std::string ResourceManager::KeyFromTexture(Texture2D texture) {
    std::string res = "None";
    for (auto& [key, t] : m_textures) {
        if (t.id != 0 && t.id == texture.id) {
            res = key;
            break;
        }
    }
    return res;
}


Texture2D *ResourceManager::GetTexture(std::string key) {
    if (m_textures.find(key) != m_textures.end()) return &m_textures[key];
    else return nullptr;
}

R3D_Model *ResourceManager::GetModel(std::string key) {
    if (ModelExists(key)) return &m_models[key];
    else return nullptr;
}

R3D_AnimationLib *ResourceManager::GetAnimationLib(std::string key) {
    if (AnimationLibExists(key)) return &m_animation_libs[key];
    else return nullptr;
}

ModelSettings *ResourceManager::GetModelSettings(std::string key) {
    if (ModelSettingsExists(key)) return &m_model_settings[key];
    else return nullptr;
}

bool ResourceManager::ModelPreviewExists(std::string key) {
    return m_model_previews.find(key) != m_model_previews.end();
}

void ResourceManager::UnloadModelPreviews() {
    for (auto& [key, preview] : m_model_previews) {
        UnloadRenderTexture(preview);
    }
    m_model_previews.clear();
}

void ResourceManager::UpdateModelPreviews() {
    std::set<std::string> keys_to_remove = {};
    for (auto& [key, preview] : m_model_previews) {
        if (!ModelExists(key)) {
            UnloadRenderTexture(preview);
            keys_to_remove.insert(key);
        }
    }
    for (auto& key : keys_to_remove) {
        m_model_previews.erase(key);
    }

    for (auto& [key, model] : m_models) {
        if (!ModelPreviewExists(key)) {
            m_model_previews[key] = LoadRenderTexture(100, 100);

            BoundingBox bb = m_models.at(key).aabb;
            float diam = Vector3Length(bb.max - bb.min);
            
            model_preview_camera.position = Vector3{diam, diam, diam};
            R3D_View view = {.camera = R3D_CameraFromRL(model_preview_camera), .target = m_model_previews.at(key)};
            R3D_BeginPro(view);
                R3D_DrawModel(m_models.at(key), Vector3{0.0f, 0.0f, 0.0f}, 1.0f);
            R3D_End();
        }
    }
}

RenderTexture2D *ResourceManager::GetModelPreview(std::string key) {
    if (ModelPreviewExists(key)) return &m_model_previews[key];
    else return nullptr;
}

LilSound *ResourceManager::GetSound(std::string key) {
    if (SoundExists(key)) return &m_sounds[key];
    else return nullptr;
}

void ResourceManager::SoundAdd(std::string key, LilSound sound) {
    if (m_sounds.find(key) != m_sounds.end()) {
        m_sounds[key].Unload();
    }
    m_sounds[key] = sound;
}

void ResourceManager::SoundAdd(std::string key, std::string filename) {
    LilSound sound;
    sound.Load(filename);
    SoundAdd(key, sound);
}

void ResourceManager::SoundAdd(std::string filename) {
    std::string key = NameFromPath(filename);
    SoundAdd(key, filename);
}

bool ResourceManager::SoundExists(std::string key) {
    return m_sounds.find(key) != m_sounds.end();
}

void ResourceManager::SoundUnload(std::string key) {
    if (SoundExists(key)) {
        GetSound(key)->Unload();
        m_sounds.erase(key);
    }
}

void ResourceManager::SoundUnloadAll() {
    for (auto& [key, sound] : m_sounds) {
        sound.Unload();
    }
    m_sounds.clear();
}
