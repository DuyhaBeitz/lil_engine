#pragma once
#include "lil_engine.hpp"

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
    const char *m_title = "lil editor";
    
    Camera3D m_camera = {0};
    bool m_cursor_enabled = true;

    void InitResources();
    void InitCamera();
    void InitPhyiscs();
};