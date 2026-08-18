#pragma once

#include "CommonIncludes.hpp"

class SceneManager {
public:
    void LoadScene(std::string filename);
    void SaveScene(std::string filename);
};