#pragma once

#include <raylib.h>
#include <raymath.h>
#include <memory>
#include "Log.hpp"
//#include "ReactPhysics3DWrapper.hpp"
#include "utils/ColorHelper.hpp"

inline std::string NameFromPath(std::string filename) {
    size_t slash_pos = filename.find_last_of("/\\");
    std::string name = (slash_pos == std::string::npos) ? filename : filename.substr(slash_pos + 1);
    return name;
}

inline std::string NameFromObject(std::string object) {
    size_t dot_pos = object.find_last_of("._>");
    std::string name = (dot_pos == std::string::npos) ? object : object.substr(dot_pos + 1);
    return name;
}

inline std::string HeightmapNameFromImageName(std::string image_name) {
    return "Heightmap_" + image_name;
}

// namespaces raylib and reactphysics3d conflict with color definitions
#define RAYLIGHTGRAY  CLITERAL(Color){ 200, 200, 200, 255 }   // Light Gray
#define RAYGRAY       CLITERAL(Color){ 130, 130, 130, 255 }   // Gray
#define RAYDARKGRAY   CLITERAL(Color){ 80, 80, 80, 255 }      // Dark Gray
#define RAYYELLOW     CLITERAL(Color){ 253, 249, 0, 255 }     // Yellow
#define RAYGOLD       CLITERAL(Color){ 255, 203, 0, 255 }     // Gold
#define RAYORANGE     CLITERAL(Color){ 255, 161, 0, 255 }     // Orange
#define RAYPINK       CLITERAL(Color){ 255, 109, 194, 255 }   // Pink
#define RAYRED        CLITERAL(Color){ 230, 41, 55, 255 }     // Red
#define RAYMAROON     CLITERAL(Color){ 190, 33, 55, 255 }     // Maroon
#define RAYGREEN      CLITERAL(Color){ 0, 228, 48, 255 }      // Green
#define RAYLIME       CLITERAL(Color){ 0, 158, 47, 255 }      // Lime
#define RAYDARKGREEN  CLITERAL(Color){ 0, 117, 44, 255 }      // Dark Green
#define RAYSKYBLUE    CLITERAL(Color){ 102, 191, 255, 255 }   // Sky Blue
#define RAYBLUE       CLITERAL(Color){ 0, 121, 241, 255 }     // Blue
#define RAYDARKBLUE   CLITERAL(Color){ 0, 82, 172, 255 }      // Dark Blue
#define RAYPURPLE     CLITERAL(Color){ 200, 122, 255, 255 }   // Purple
#define RAYVIOLET     CLITERAL(Color){ 135, 60, 190, 255 }    // Violet
#define RAYDARKPURPLE CLITERAL(Color){ 112, 31, 126, 255 }    // Dark Purple
#define RAYBEIGE      CLITERAL(Color){ 211, 176, 131, 255 }   // Beige
#define RAYBROWN      CLITERAL(Color){ 127, 106, 79, 255 }    // Brown
#define RAYDARKBROWN  CLITERAL(Color){ 76, 63, 47, 255 }      // Dark Brown

#define RAYBLACK      CLITERAL(Color){ 0, 0, 0, 255 }         // Black
#define RAYBLANK      CLITERAL(Color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define RAYMAGENTA    CLITERAL(Color){ 255, 0, 255, 255 }     // Magenta