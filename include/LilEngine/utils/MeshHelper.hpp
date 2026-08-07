#pragma once

#include "CommonIncludes.hpp"

R3D_Model LoadR3DModelFromMesh(R3D_Mesh mesh);
R3D_Model HeightmapModel(Image& image, Vector3 size);