#pragma once

#include "LilEngine.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

inline const char*  BrowseTextureDialog() {   
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

inline const char* BrowseModelDialog() {
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

inline const char* BrowseSoundDialog() {
    const char* filters[] = { "*.wav", "*.ogg", "*.mp3" };
    const char* outPath = tinyfd_openFileDialog(
        "Select Sound",
        "",
        3,
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