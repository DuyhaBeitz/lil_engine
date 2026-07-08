#pragma once
#include "CommonIncludes.hpp"
#include "World.hpp"
#include "ResourceManager.hpp"

class App {
public:
    static App& Get() {
        static App instance;
        return instance;
    }

    bool Init();
    void Update();
    void Draw();

private:
    const char *m_title = "Datum";
    
    Camera3D m_camera = {0};
    World m_world;
    bool m_cursor_enabled = true;

    void InitResources();
    void InitCamera();
    void InitPhyiscs();
};