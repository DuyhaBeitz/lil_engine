#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>

class ResourceManager {
public:

    void Unload();

    void TextureAdd(std::string key, Texture2D texture);
    void TextureAdd(std::string key, std::string filename);
    void TextureAdd(std::string filename);
    bool TextureExists(std::string key);
    void TextureUnload(std::string key);
    void TextureUnloadAll();

    void ModelAdd(std::string key, Model model);
    void ModelAdd(std::string key, std::string filename);
    void ModelAdd(std::string filename);
    bool ModelExists(std::string key);
    void ModelUnload(std::string key);
    void ModelUnloadAll();

    Texture2D* GetTexture(std::string key) {
        if (m_textures.find(key) != m_textures.end()) return &m_textures[key];
        else return nullptr;
    }

    Model* GetModel(std::string key) {
        if (m_models.find(key) != m_models.end()) return &m_models[key];
        else {
            return nullptr;
        };
    }

    std::unordered_map<std::string, Texture2D>& Textures() { return m_textures; }
    std::unordered_map<std::string, Model>& Models() { return m_models; }

private:
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_map<std::string, Model> m_models;
};