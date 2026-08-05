#pragma once

#include "LilEngine.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

inline const char*  BrowseTexture() {   
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
        return outPath;
    }
    else {
        LIL_LOG_INFO("User pressed cancel or an error occurred.");
        return nullptr;
    }
}

inline const char* BrowseModel() {
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
        return outPath;
    }
    else {
        LIL_LOG_INFO("User pressed cancel or an error occurred.");
        return nullptr;
    }
}

inline const char* BrowseSound() {
    const char* filters[] = { "*.wav", "*.ogg", "*.mp3" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Sound",
        "",
        1,
        filters,
        "Sound files",
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