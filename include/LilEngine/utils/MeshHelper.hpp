#pragma once

#include "CommonIncludes.hpp"

inline Model HeightmapModel(Image& image, Vector3 size) {
    Texture2D texture = LoadTextureFromImage(image);        // Convert image to texture (VRAM)

    Mesh mesh = GenMeshHeightmap(image, size); // Generate heightmap mesh (RAM and VRAM)
    Model model = LoadModelFromMesh(mesh);                  // Load model from generated mesh

    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture
    return model;
}

inline float MaxRad(const Model& model) {
    float max_rad = 0.f;
    for (int i = 0; i < model.meshCount; i++) {
        Mesh mesh = model.meshes[i];
        for (int j = 0; j < mesh.vertexCount; j++) {
            float x = mesh.vertices[j*3];
            float y = mesh.vertices[j*3+1];
            float z = mesh.vertices[j*3+2];
            Vector3 v = {x, y, z};
            max_rad = std::max(max_rad, Vector3Length(v));
        }
    }
    return max_rad;
}