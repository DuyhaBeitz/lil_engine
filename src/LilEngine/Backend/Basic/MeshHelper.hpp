#pragma once

#include <lil3d/lil3d.h>

// Builds a single-mesh terrain model from a heightmap image.
// Returns a zeroed model if the image is unusable.
inline LIL3D_Model HeightmapModel(Image &image, Vector3 size) {
    LIL3D_Mesh mesh = LIL3D_GenMeshHeightmap(image, size);
    if (mesh.mesh.vertexCount <= 0) return LIL3D_Model{};
    return LIL3D_LoadModelFromMesh(mesh);
}
