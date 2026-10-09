#pragma once

#include "CommonIncludes.hpp"

class SceneManager {
private:
    void UpdateScene();
public:
    void ClearScene();
    void LoadScene(std::string filename);
    void SaveScene(std::string filename);
};