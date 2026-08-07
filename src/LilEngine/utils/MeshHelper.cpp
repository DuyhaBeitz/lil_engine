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

    R3D_Mesh mesh = R3D_GenMeshHeightmap(image, size);
    R3D_Model model = LoadR3DModelFromMesh(mesh);

    //model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set map diffuse texture
    return model;
}

void DrawR3DModelWiresEx(R3D_Model model, Vector3 position, Quaternion rotation, Vector3 scale) {
    rlEnableWireMode();
    R3D_DrawModelEx(model, position, rotation, scale);
    rlDisableWireMode();
}

void DrawR3DModelWires(R3D_Model model, Vector3 position, float scale) {
    rlEnableWireMode();
    R3D_DrawModel(model, position, scale);
    rlDisableWireMode();
}