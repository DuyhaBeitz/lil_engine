#pragma once

#include "LilEngine.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> SplitPaths(const char* browse_result) {
    std::vector<std::string> paths;

    if (browse_result) {
        std::stringstream ss(browse_result);
        std::string token;

        while (std::getline(ss, token, '|')) {
            if (!token.empty()) {
                paths.push_back(token);
            }
        }
    }
    else {
        LIL_LOG_INFO("User pressed cancel or an error occurred.");
    }
    return paths;
}

inline std::vector<std::string>  BrowseTextureDialog() {   
    const char* filters[] = { "*.png", "*.jpeg", "*.jpg" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Texture",           // Title
        "",                          // Default path (empty = current directory)
        3,                          // Number of filters
        filters,                    // Filter array
        "Image files",              // Filter description
        1                           // Allow multiple selections (0 = single)
    );

    return SplitPaths(outPath);
}

inline std::vector<std::string> BrowseModelDialog() {
    const char* filters[] = { "*.glb" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Model",
        "",
        1,
        filters,
        "Model files",
        1
    );

    return SplitPaths(outPath);
}

inline std::vector<std::string> BrowseSoundDialog() {
    const char* filters[] = { "*.wav", "*.ogg", "*.mp3" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Sound",
        "",
        3,
        filters,
        "Sound files",
        1
    );

    return SplitPaths(outPath);
}

inline const char* BrowseSceneDialog() {
    const char* filters[] = { "*.json"};
    const char* outPath = tinyfd_openFileDialog(
        "Select scene",
        "",
        1,
        filters,
        "Scene files",
        0
    );

    if (outPath) {
        return outPath;
    }
    else {
        LIL_LOG_INFO("User pressed cancel or an error occurred.");
        return nullptr;
    }
}

inline const char* SaveSceneDialog() {
    const char* filters[] = { "*.json" };

    const char* outPath = tinyfd_saveFileDialog(
        "Save scene",
        "scene.json",
        1,
        filters,
        "Scene files"
    );

    if (outPath) {
        return outPath;
    } else {
        LIL_LOG_INFO("User pressed cancel or an error occurred.");
        return nullptr;
    }
}