#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>

class ResourceManager {
public:
    static ResourceManager& Get() {
        static ResourceManager instance;
        return instance;
    }
    
    /*************************************************** */

    void AddTexture(std::string key, Texture2D texture);
    void AddTexture(std::string key, std::string filename);
    void AddTexture(std::string filename);

    /*************************************************** */

    void AddModel(std::string key, Model model);
    void AddModel(std::string key, std::string filename);
    void AddModel(std::string filename);

    Texture2D* GetTextrue(std::string key) {
        if (m_textures.find(key) != m_textures.end()) return &m_textures[key];
        else return nullptr;
    }

    Model* GetModel(std::string key) {
        if (m_models.find(key) != m_models.end()) return &m_models[key];
        else {
            return nullptr;
        };
    }

    std::unordered_map<std::string, Texture2D>* Textures() { return &m_textures; }
    std::unordered_map<std::string, Model>* Models() { return &m_models; }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_map<std::string, Model> m_models;
};