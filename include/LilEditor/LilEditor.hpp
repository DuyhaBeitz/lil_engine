#pragma once

#include "LilEngine.hpp"
#include "Notification.hpp"
#include "EditorUI.hpp"
#include "Painter.hpp"

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
        Lil::Editor::Get().Close();
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
        void Close();

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

        std::string m_loaded_scene = "";

        bool m_cursor_enabled = true;
        bool m_physics_debug = false;
        Actor* m_selected_actor = nullptr;
        Component* m_selected_component = nullptr;
        Painter m_painter;

        RenderTexture2D m_viewport_render_target;
        RenderTexture2D m_layout_render_target;

        EditorUIVisitor m_editor;
        GizmoFlags m_gizmo_mode = GIZMO_TRANSLATE;
        GizmoFlags m_gizmo_space = GIZMO_DISABLED;

        void ReadConfig();
        void WriteConfig();

        void DropSelectedActor();
        void DropSelectedComponent();
        void SelectActor(Actor* actor);

        void LoadScene(std::string filename);
        void SaveScene(std::string filename);

        void ClearScene();
        void LoadScene();
        void SaveScene();

        void InitUI();
        void DrawMenuBar();

        void DrawInspector();
        void DrawComponents();
        void DrawLayout();
        void DrawPainter();

        void UpdateGizmoMode();
        void HandleViewportInput();
        void DrawViewport();

        void DrawResources();

        void DrawEnvironment();

        void Notify(const std::string& message, float duration = 3.0f, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        void DrawNotifications();
    };
}