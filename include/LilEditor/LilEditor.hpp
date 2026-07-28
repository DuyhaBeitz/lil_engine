#pragma once

#include "LilEngine.hpp"
#include "EditorUI.hpp"

#include "raygizmo.h"
#include "imgui.h"
#include "rlImGui.h"

/*
Usage:

int main() {
    void LoadMyResources();
    Lil::Editor::Get().Init();
    if (with_editor) {
        Lil::Editor::Get().Init();
        while (!WindowShouldClose()) {
            Lil::Editor::Get().Update();
            BeginDrawing();
            Lil::Editor::Get().Draw();
            EndDrawing();
        }
    }
    else {
        while (!WindowShouldClose()) {
            MyGameUpdate();
            BeginDrawing();
            MyGameDraw();
            EndDrawing();
        }
    }
}

*/

namespace Lil {
    class Editor {
    public:
        static Editor& Get();
        
        void Init();
        void Update();
        void Draw();

    private:
        Camera m_camera = {
            .position = (Vector3){ 10.0f,10.0f, 10.0f },  // Camera position
            .target = (Vector3){ 0.0f, 0.0f, 0.0f },      // Camera looking at point
            .up = (Vector3){ 0.0f, 1.0f, 0.0f },          // Camera up vector (rotation towards target)
            .fovy = 60.0f,                                // Camera field-of-view Y
            .projection = CAMERA_PERSPECTIVE              // Camera mode type
        };
        bool m_cursor_enabled = true;
        Actor* m_selected = nullptr;
        RenderTexture2D m_render_target;
        EditorUIVisitor m_editor;
        GizmoFlags m_gizmo_mode = GIZMO_TRANSLATE;
        GizmoFlags m_gizmo_space = GIZMO_DISABLED;
        
        void InitUI();
        void TryResizeTarget(int w, int h);
        void LoadTarget(int w, int h);
        void DrawTarget();

        void DrawInspector();

        void HandleViewportInput();
        void DrawViewport();

        void DrawResources();
    };
}