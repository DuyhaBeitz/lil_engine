#include "ResourceManager.hpp"
#include <set>

Camera3D model_preview_camera = {
    .position = (Vector3){ 10.0f,10.0f, 10.0f },  // Camera position
    .target = (Vector3){ 0.0f, 0.0f, 0.0f },      // Camera looking at point
    .up = (Vector3){ 0.0f, 1.0f, 0.0f },          // Camera up vector (rotation towards target)
    .fovy = 60.0f,                                // Camera field-of-view Y
    .projection = CAMERA_PERSPECTIVE              // Camera mode type
};

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
}

void ResourceManager::ModelAdd(std::string key, std::string filename) {
    R3D_Model model = R3D_LoadModelEx(filename.c_str(), R3D_IMPORT_MESH_DATA);
    ModelAdd(key, model);
}

void ResourceManager::ModelAdd(std::string filename) {
    std::string key = NameFromPath(filename);
    ModelAdd(key, filename);
}

bool ResourceManager::ModelExists(std::string key) {
    return m_models.find(key) != m_models.end();
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

Texture2D *ResourceManager::GetTexture(std::string key) {
    if (m_textures.find(key) != m_textures.end()) return &m_textures[key];
    else return nullptr;
}

R3D_Model *ResourceManager::GetModel(std::string key) {
    if (ModelExists(key)) return &m_models[key];
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
        }
        
        BoundingBox bb = m_models.at(key).aabb;
        float diam = Vector3Length(bb.max - bb.min);
        
        model_preview_camera.position = Vector3{diam*cosf(GetTime()), diam, diam*sinf(GetTime())};
        BeginTextureMode(m_model_previews.at(key));
            ClearBackground(RAYBLACK);
            R3D_Begin(model_preview_camera);
                R3D_DrawModel(m_models.at(key), Vector3{0.0f, 0.0f, 0.0f}, 1.0f);
            R3D_End();
        EndTextureMode();
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
