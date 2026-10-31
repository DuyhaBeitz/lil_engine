#include "Render/Frame.hpp"
#include <r3d/r3d.h>

namespace Lil::Render {
    ::Camera3D* s_camera = nullptr;
    ::Rectangle s_viewport;
    ::RenderTexture* s_target = nullptr;

    void InitWindow(int width, int height, std::string title) {
        ::InitWindow(width, height, title.c_str());
        R3D_Init(GetScreenWidth(), GetScreenHeight());
        InitAudioDevice();
    }

    void CloseWindow() {
        CloseAudioDevice();
        ::CloseWindow();
    }

    void SetResolution(int width, int height) {
        R3D_SetResolution(width, height);
    }

    void BeginView(::Camera3D &camera, ::Rectangle viewport, ::RenderTexture *target) {
        s_camera = &camera;
        s_viewport = viewport;
        s_target = target;
    }

    void BeginScene() {
        if (!s_camera) return;
        R3D_View view = {
            .camera = R3D_CameraFromRL(*s_camera),
            .target = (s_target != nullptr) ? *s_target : RenderTexture{0},
            .viewport = s_viewport
        };
        R3D_BeginPro(view);
    }

    void EndScene() {
        if (!s_camera) return;
        R3D_End();
    }

    void BeginOverlay() {
        if (!s_camera) return;
        if (s_target != nullptr) BeginTextureMode(*s_target);
        BeginMode3D(*s_camera);
    }

    void EndOverlay() {
        if (!s_camera) return;
        EndMode3D();
        if (s_target != nullptr) EndTextureMode();
    }

    void EndView() {
        s_camera = nullptr;
        s_target = nullptr;
    }

}; // namespace Lil