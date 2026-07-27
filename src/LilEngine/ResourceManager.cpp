#include "ResourceManager.hpp"

void ResourceManager::Unload() {
    ModelUnloadAll();
    TextureUnloadAll();
}

void ResourceManager::TextureAdd(std::string key, Texture2D texture)
{
    if (m_textures.find(key) != m_textures.end()) {
        UnloadTexture(m_textures[key]);
    }
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
    for (auto& [key, texture] : m_textures) {
        UnloadTexture(texture);
    }
    m_textures.clear();
}

void ResourceManager::ModelAdd(std::string key, Model model) {
    if (m_models.find(key) != m_models.end()) {
        UnloadModel(m_models[key]);
    }
    m_models[key] = model;
}

void ResourceManager::ModelAdd(std::string key, std::string filename) {
    Model model = LoadModel(filename.c_str());
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
        UnloadModel(*GetModel(key));
        m_models.erase(key);
    }
}

void ResourceManager::ModelUnloadAll() {
    for (auto& [key, model] : m_models) {
        UnloadModel(model);
    }
    m_models.clear();
}
