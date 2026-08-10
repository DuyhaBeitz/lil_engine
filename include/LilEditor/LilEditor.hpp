#pragma once

#include "LilEngine.hpp"
#include "Notification.hpp"
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
        std::vector<Notification> m_notifications;
        Camera m_viewport_camera = {
            .position = (Vector3){ 10.0f,10.0f, 10.0f },
            .target = (Vector3){ 0.0f, 0.0f, 0.0f },
            .up = (Vector3){ 0.0f, 1.0f, 0.0f },
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        };
        Camera m_layout_camera = {
            .position = (Vector3){ 10.0f,10.0f, 10.0f },
            .target = (Vector3){ 0.0f, 0.0f, 0.0f },
            .up = (Vector3){ 0.0f, 1.0f, 0.0f },
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        };

        bool m_cursor_enabled = true;
        Actor* m_selected_actor = nullptr;
        Component* m_selected_component = nullptr;

        RenderTexture2D m_viewport_render_target;
        RenderTexture2D m_layout_render_target;

        EditorUIVisitor m_editor;
        GizmoFlags m_gizmo_mode = GIZMO_TRANSLATE;
        GizmoFlags m_gizmo_space = GIZMO_DISABLED;

        void DropSelectedActor();
        void SelectActor(Actor* actor);

        void LoadScene();
        void SaveScene();

        void InitUI();
        void DrawMenuBar();

        void DrawInspector();
        void DrawLayout();

        void UpdateGizmoMode();
        void HandleViewportInput();
        void DrawViewport();

        void DrawResources();

        void DrawEnvironment();

        void Notify(const std::string& message, float duration = 3.0f, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        void DrawNotifications();
    };
}