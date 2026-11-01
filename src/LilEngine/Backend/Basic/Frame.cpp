#include "Render/Frame.hpp"
#include <lil3d/lil3d.h>

namespace Lil::Render {
    ::Camera3D* s_camera = nullptr;
    ::Rectangle s_viewport;
    ::RenderTexture* s_target = nullptr;

    void InitWindow(int width, int height, std::string title) {
        ::InitWindow(width, height, title.c_str());
        if (!LIL3D_Init(GetScreenWidth(), GetScreenHeight()))
            TraceLog(LOG_ERROR, "Render: lighting backend failed to initialise");
        InitAudioDevice();
    }

    void CloseWindow() {
        CloseAudioDevice();
        LIL3D_Close();
        ::CloseWindow();
    }

    // The backend renders straight into the active framebuffer / target,
    // so there is no internal resolution to change.
    void SetResolution(int width, int height) {
        (void)width; (void)height;
    }

    void BeginView(::Camera3D &camera, ::Rectangle viewport, ::RenderTexture *target) {
        s_camera = &camera;
        s_viewport = viewport;   // not supported by the backend: the whole target is rendered
        s_target = target;
    }

    void BeginScene() {
        if (!s_camera) return;
        if (s_target != nullptr) LIL3D_BeginEx(*s_camera, *s_target);
        else                     LIL3D_Begin(*s_camera);
    }

    void EndScene() {
        if (!s_camera) return;
        LIL3D_End();
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
