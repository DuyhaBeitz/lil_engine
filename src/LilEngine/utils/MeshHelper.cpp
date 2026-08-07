#include "utils/MeshHelper.hpp"
#include <rlgl.h>

R3D_Model LoadR3DModelFromMesh(R3D_Mesh mesh) {
    R3D_Model model = {0};

    model.meshes = reinterpret_cast<R3D_Mesh*>(RL_MALLOC(sizeof(R3D_Mesh)));
    model.meshes[0] = mesh;
    model.meshCount = 1;

    model.materials = reinterpret_cast<R3D_Material*>(RL_MALLOC(sizeof(R3D_Material)));
    model.materials[0] = R3D_GetDefaultMaterial();
    model.materialCount = 1;

    model.meshMaterials = reinterpret_cast<int*>(RL_MALLOC(sizeof(int)));
    model.meshMaterials[0] = 0;

    model.aabb = mesh.aabb;
    
    return model;
}

R3D_Model HeightmapModel(Image &image, Vector3 size) {
    Texture2D texture = LoadTextureFromImage(image);

    R3D_Model model = {0};
    R3D_Mesh mesh = {0};
    R3D_MeshData data = R3D_GenMeshDataHeightmap(image, size);
    if (!R3D_IsMeshDataValid(data)) return model;

    BoundingBox aabb = {
        {-size.x * 0.5f,   0.0f, -size.z * 0.5f},
        { size.x * 0.5f, size.y,  size.z * 0.5f}
    };

    mesh = R3D_LoadMesh(R3D_PRIMITIVE_TRIANGLES, data, &aabb);
    model = LoadR3DModelFromMesh(mesh);

    model.meshData = reinterpret_cast<R3D_MeshData*>(RL_MALLOC(sizeof(R3D_MeshData)));
    *(model.meshData) = data;

    //model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture
    return model;
}