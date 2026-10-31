#pragma once
#include <raylib.h>
#include <string>

namespace Lil::Render {
    void InitWindow(int width, int height, std::string title);
    void CloseWindow();

    void SetResolution(int width, int height);
    void BeginView(::Camera3D& camera, ::Rectangle viewport, ::RenderTexture* target = nullptr);
    void   BeginScene();
    void   EndScene();
    void   BeginOverlay();
    void   EndOverlay();
    void EndView(); 

}; // namespace Lil