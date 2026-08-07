#pragma once

#include "CommonIncludes.hpp"

R3D_Model LoadR3DModelFromMesh(R3D_Mesh mesh);
R3D_Model HeightmapModel(Image& image, Vector3 size);

void DrawR3DModelWiresEx(R3D_Model model, Vector3 position, Quaternion rotation, Vector3 scale);
void DrawR3DModelWires(R3D_Model model, Vector3 position, float scale);