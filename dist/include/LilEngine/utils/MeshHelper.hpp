#pragma once

#include <r3d/r3d.h>

R3D_Model LoadR3DModelFromMesh(R3D_Mesh mesh);
R3D_Model HeightmapModel(Image& image, Vector3 size);