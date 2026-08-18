#include "utils/NameHelper.hpp"

std::string NameFromPath(std::string filename) {
    size_t slash_pos = filename.find_last_of("/\\");
    std::string name = (slash_pos == std::string::npos) ? filename : filename.substr(slash_pos + 1);
    return name;
}

std::string NameFromObject(std::string object) {
    size_t dot_pos = object.find_last_of("._>");
    std::string name = (dot_pos == std::string::npos) ? object : object.substr(dot_pos + 1);
    return name;
}

bool IsAssetGenerated(std::string asset_key) {
    return asset_key.length() >= 4 && (asset_key.substr(0, 4) == "Gen_");
}

std::string HeightmapNameFromImageName(std::string image_name) {
    return "Gen_Heightmap_From_" + image_name;
}
