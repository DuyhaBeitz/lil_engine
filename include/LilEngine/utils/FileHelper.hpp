#pragma once

#include "CommonIncludes.hpp"

inline void CopyFile(std::string source, std::string dest) {
    int dataSize = 0;
    unsigned char *fileData = LoadFileData(source.c_str(), &dataSize);

    if (fileData == NULL) return;

    SaveFileData(dest.c_str(), fileData, dataSize);
}

inline void CopyAsset(std::string source) {
    CopyFile(source, "assets/" + NameFromPath(source));
}