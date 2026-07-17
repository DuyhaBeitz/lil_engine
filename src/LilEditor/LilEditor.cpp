#include "LilEditor.hpp"

Lil::Editor &Lil::Editor::Get() {
    static Lil::Editor instance;
    return instance;
}

#define TOGGLE_CURSOR_KEY KEY_L
#define TOGGLE_SIMULATION_KEY KEY_F
#define TOGGLE_DEBUG_KEY KEY_V
#define TOGGLE_FULLSCREEN_KEY KEY_F11

void Lil::Editor::InitUI() {
	rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    
    float s = 3.0f;
    io.DisplayFramebufferScale = ImVec2(s, s);
    io.FontGlobalScale = s;
#ifdef IMGUI_HAS_DOCK
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
}

void Lil::Editor::LoadTarget(int w, int h) {
    m_render_target = LoadRenderTexture(w, h);
}

void Lil::Editor::TryResizeTarget(int w, int h) {
    if (m_render_target.texture.width != w || m_render_target.texture.height != h) {
        LoadTarget(w, h);
    }
}

void Lil::Editor::Init() {
    LoadTarget(GetScreenWidth(), GetScreenHeight());
    InitUI();
}

void Lil::Editor::DrawTarget() {
    BeginTextureMode(m_render_target);
        ClearBackground(RAYBLACK);
        
        BeginMode3D(m_camera);
            Lil::GetWorld().Draw();
            if (Lil::GetWorld().m_physics_debug) Lil::GetWorld().DebugDraw();
            if (m_selected) {
                Transform t = m_selected->GetTransform();
                DrawGizmo3D(m_gizmo_mode | m_gizmo_space, &t);
                m_selected->SetTransform(t);
            }
            if (m_cursor_enabled && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Actor* pick = Lil::GetWorld().PickActor(GetMousePosition(), m_render_target.texture.width, m_render_target.texture.height, m_camera);
                if (pick) m_selected = pick;
            }
        EndMode3D();
    EndTextureMode();
}

void Lil::Editor::Update() {
    if (!m_cursor_enabled) {UpdateCamera(&m_camera, CAMERA_FREE);}
    Lil::GetWorld().Update();

    if (IsKeyPressed(TOGGLE_CURSOR_KEY)) {
        if (m_cursor_enabled) DisableCursor();
        else EnableCursor();
        m_cursor_enabled = !m_cursor_enabled;
    }
    if (IsKeyPressed(TOGGLE_SIMULATION_KEY)) {
        Lil::GetWorld().ToggleSimulationGoing();
    }
    if (IsKeyPressed(TOGGLE_DEBUG_KEY)) {
        Lil::GetWorld().m_physics_debug = !Lil::GetWorld().m_physics_debug;
    }

    if (IsKeyPressed(TOGGLE_FULLSCREEN_KEY)) {
        ToggleBorderlessWindowed();
    }
    if (IsKeyPressed(KEY_ESCAPE)) m_selected = nullptr;

    if (IsKeyPressed(KEY_G)) m_gizmo_mode = GIZMO_TRANSLATE;
    if (IsKeyPressed(KEY_R)) m_gizmo_mode = GIZMO_ROTATE;
    if (IsKeyPressed(KEY_S)) m_gizmo_mode = GIZMO_SCALE;
}

void Lil::Editor::DrawViewportUI() {
    ImGui::Begin("Viewport");
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 viewerTopLeft = ImGui::GetCursorScreenPos();
    
    SetMouseOffset(-viewerTopLeft.x, -viewerTopLeft.y);

    TryResizeTarget(contentSize.x, contentSize.y);
    SetGizmoRenderSize(contentSize.x, contentSize.y);

    DrawTarget();
    rlImGuiImageRenderTexture(&m_render_target);
    
    SetMouseOffset(0, 0);
    
    ImGui::End();
}

void Lil::Editor::Draw() {
    ClearBackground(RAYBLACK);
    rlImGuiBegin();

#ifdef IMGUI_HAS_DOCK
    ImGui::DockSpaceOverViewport(0,  NULL, ImGuiDockNodeFlags_PassthruCentralNode); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
#endif
    ImGui::Begin("Inspector");
    if (m_selected) m_selected->GetTypeInfo().VisitFields(m_selected, m_editor);
    ImGui::End();
    
    DrawViewportUI();

    ImGui::Begin("Panel");
    if (m_gizmo_space == GIZMO_LOCAL) {
        if (ImGui::Button("global")) m_gizmo_space = GIZMO_DISABLED; 
    }
    else if (ImGui::Button("local")) m_gizmo_space = GIZMO_LOCAL; 

    ImGui::End();

    ImGui::Begin("Actors");
    ImGui::End();

    rlImGuiEnd();
}
