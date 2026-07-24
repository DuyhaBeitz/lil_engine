#pragma once

#include "LilEngine.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

inline void BrowseTexture() {   
    const char* filters[] = { "*.png", "*.jpeg", "*.jpg" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Texture",           // Title
        "",                          // Default path (empty = current directory)
        3,                          // Number of filters
        filters,                    // Filter array
        "Image files",              // Filter description
        0                           // Allow multiple selections (0 = single)
    );

    if (outPath) {
        std::string filename = outPath;
        Lil::Resources().AddTexture(NameFromPath(filename), filename);
    }
    else {
        LOG_INFO("User pressed cancel or an error occurred.");
    }
}

inline void BrowseModel() {
    const char* filters[] = { "*.glb" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Model",
        "",
        1,
        filters,
        "Model files",
        0
    );

    if (outPath) {
        std::string filename = outPath;
        Lil::Resources().AddModel(NameFromPath(filename), filename);
    }
    else {
        LOG_INFO("User pressed cancel or an error occurred.");
    }
}