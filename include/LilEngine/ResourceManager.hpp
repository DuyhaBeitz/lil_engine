#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>

class ResourceManager {
private:
    std::unordered_map<std::string, Texture2D> m_textures;

    std::unordered_map<std::string, Model> m_models;
    std::unordered_map<std::string, RenderTexture2D> m_model_previews;

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

    bool ModelPreviewExists(std::string key);
    void UnloadModelPreviews();
    void UpdateModelPreviews();

    Texture2D* GetTexture(std::string key);
    Model* GetModel(std::string key);
    RenderTexture2D* GetModelPreview(std::string key);

    std::unordered_map<std::string, Texture2D>& Textures() { return m_textures; }
    std::unordered_map<std::string, Model>& Models() { return m_models; }
};