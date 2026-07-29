#pragma once

#include "CommonIncludes.hpp"

class SceneManager {
private:
public:
    void LoadScene(std::string filename);
    void SaveScene(std::string filename);
};