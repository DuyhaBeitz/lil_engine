#include "ResourceManager.hpp"

void ResourceManager::AddTexture(std::string key, Texture2D texture) {
    if (m_textures.find(key) != m_textures.end()) {
        UnloadTexture(m_textures[key]);
    }
    m_textures[key] = texture;
}

void ResourceManager::AddTexture(std::string key, std::string filename) {
    Texture2D texture = LoadTexture(filename.c_str());
    AddTexture(key, texture);
}

void ResourceManager::AddTexture(std::string filename) {
    std::string key = NameFromPath(filename);
    AddTexture(key, filename);
}

bool ResourceManager::TextureExists(std::string key) {
    return m_textures.find(key) != m_textures.end();
}

void ResourceManager::AddModel(std::string key, Model model) {
    if (m_models.find(key) != m_models.end()) {
        UnloadModel(m_models[key]);
    }
    m_models[key] = model;
}

void ResourceManager::AddModel(std::string key, std::string filename) {
    Model model = LoadModel(filename.c_str());
    AddModel(key, model);
}

void ResourceManager::AddModel(std::string filename) {
    std::string key = NameFromPath(filename);
    AddModel(key, filename);
}

bool ResourceManager::ModelExists(std::string key) {
    return m_models.find(key) != m_models.end();
}