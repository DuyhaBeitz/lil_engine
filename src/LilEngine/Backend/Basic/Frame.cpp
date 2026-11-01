#include "Render/Frame.hpp"
#include <r3d/r3d.h>

namespace Lil::Render {

    ::Camera3D* s_camera = nullptr;
    ::Rectangle s_viewport;
    ::RenderTexture* s_target = nullptr;

    void InitWindow(int width, int height, std::string title) {
        ::InitWindow(width, height, title.c_str());
        InitAudioDevice();
    }

    void CloseWindow() {
        CloseAudioDevice();
        ::CloseWindow();
    }

    void SetResolution(int width, int height) {}

    void BeginView(::Camera3D &camera, ::Rectangle viewport, ::RenderTexture *target) {
        s_camera = &camera;
        s_viewport = viewport;
        s_target = target;
    }

    void BeginScene() {
        if (!s_camera) return;
        if (s_target != nullptr) BeginTextureMode(*s_target);
        ClearBackground(BLACK);
        BeginMode3D(*s_camera);
    }

    void EndScene() {
        if (!s_camera) return;
        EndMode3D();
        if (s_target != nullptr) EndTextureMode();
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