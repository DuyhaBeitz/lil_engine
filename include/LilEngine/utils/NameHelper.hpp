#pragma once
#include <string>

std::string NameFromPath(std::string filename);
std::string NameFromObject(std::string object);
bool IsAssetGenerated(std::string asset_key);
std::string HeightmapNameFromImageName(std::string image_name);