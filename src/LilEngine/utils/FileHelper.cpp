#include "utils/FileHelper.hpp"
#include "utils/NameHelper.hpp"
#include <raylib.h>
#include <string>
#include "utils/FileHelper.hpp"
#include <fstream>
#include <sstream>

void CopyFile(std::string source, std::string dest) {
    int dataSize = 0;
    unsigned char *fileData = LoadFileData(source.c_str(), &dataSize);

    if (fileData == NULL) return;

    SaveFileData(dest.c_str(), fileData, dataSize);
    UnloadFileData(fileData);
}
void CopyAsset(std::string source) {
    CopyFile(source, "assets/" + NameFromPath(source));
}

std::string ReadFile(std::string source) {
    std::ifstream file(source);

    if (!file) throw std::runtime_error("Could not open file for read: " + source);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void WriteFile(std::string content, std::string dest) {
    std::ofstream file(dest);
    if (!file) throw std::runtime_error("Could not open file for write: " + dest);
    file << content;
}