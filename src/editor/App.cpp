#include "App.hpp"
#include "Log.hpp"
#include "raygizmo.h"

#include "utils/ColliderHelper.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/ColliderComponent.hpp"

#include "EditorUI.hpp"

EditorUIVisitor editor;
RenderTexture2D render_target;

bool App::Init() {
    InitWindow(1920, 1080, m_title);
    SetTargetFPS(10);
    SetExitKey(KEY_NULL);
    if (!IsWindowReady()) {
        LOG_ERROR("Failed to initialize raylib");
        return false;
    }
    else {
        LOG_INFO("Successfully initialized raylib");
    }
    SetTargetFPS(60);
    //ToggleFullscreen();
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    InitResources();
    InitCamera();
    InitPhyiscs();
    InitUI();

    LoadTarget(GetScreenWidth(), GetScreenHeight());

    return true;
}

void App::InitResources() {
    Lil::Resources().AddTexture("assets/heightmap.png");
    Lil::Resources().AddModel("assets/Car.glb");
    Lil::Resources().AddModel("assets/Earth.glb");
    Lil::Resources().AddModel("assets/CesiumMan.glb");
}

void App::InitCamera() {
    m_camera.position = (Vector3){ 10.0f,10.0f, 10.0f };  // Camera position
    m_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 60.0f;                                // Camera field-of-view Y
    m_camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
}

void App::InitPhyiscs() {
    {
    auto car = Lil::GetWorld().CreateActor<Actor>();

    auto m = Lil::GetWorld().CreateComponent<ModelComponent>("Car.glb");
    car->AttachComponent(m);

    auto c = Lil::GetWorld().CreateComponent<ColliderComponent>(rc::BodyType::DYNAMIC);
    auto b = c->GetBody();
    AddBoundsBoxColliderToBody(b, *m->GetModel());
    car->AttachComponent(c);
    car->SetPosition({0, 15, 0});
    }

    Vector3 map_size = {128, 4, 128};
    auto map = Lil::GetWorld().CreateActor<Heightmap>("heightmap.png", map_size);
}

void App::InitUI() {
	rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    
    float s = 3.0f;
    io.DisplayFramebufferScale = ImVec2(s, s);
    io.FontGlobalScale = s;
#ifdef IMGUI_HAS_DOCK
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
}

Actor* selected = nullptr;

void App::Update() {
    if (!m_cursor_enabled) {UpdateCamera(&m_camera, CAMERA_FREE);}
    Lil::GetWorld().Update();

    if (IsKeyPressed(KEY_L)) {
        if (m_cursor_enabled) DisableCursor();
        else EnableCursor();
        m_cursor_enabled = !m_cursor_enabled;
    }
    if (IsKeyPressed(KEY_F)) {
        Lil::GetWorld().ToggleSimulationGoing();
    }
    if (IsKeyPressed(KEY_V)) {
        Lil::GetWorld().m_physics_debug = !Lil::GetWorld().m_physics_debug;
    }

    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }
    if (IsKeyPressed(KEY_ESCAPE)) selected = nullptr;
}
void App::Draw() {


    BeginDrawing();
        ClearBackground(RAYBLACK);

		rlImGuiBegin();
		
#ifdef IMGUI_HAS_DOCK
		ImGui::DockSpaceOverViewport(0,  NULL, ImGuiDockNodeFlags_PassthruCentralNode); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
#endif

        ImGui::Begin("Inspector");
        if (selected) selected->GetTypeInfo().VisitFields(selected, editor);
        ImGui::End();
        
        ImGui::Begin("Viewport");
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 viewerTopLeft = ImGui::GetCursorScreenPos();
        
        SetMouseOffset(-viewerTopLeft.x, -viewerTopLeft.y);

        TryResizeTarget(contentSize.x, contentSize.y);
        SetGizmoRenderSize(contentSize.x, contentSize.y);

        DrawTarget();
        rlImGuiImageRenderTexture(&render_target);
        SetMouseOffset(0, 0);
        
        ImGui::End();
	
		rlImGuiEnd();

    EndDrawing();
}

void App::TryResizeTarget(int w, int h) {
    if (render_target.texture.width != w || render_target.texture.height != h) {
        LoadTarget(w, h);
    }
}

void App::LoadTarget(int w, int h)
{
    render_target = LoadRenderTexture(w, h);
}

void App::DrawTarget()
{
    BeginTextureMode(render_target);
        ClearBackground(RAYBLACK);
        
        BeginMode3D(m_camera);
            Lil::GetWorld().Draw();
            if (Lil::GetWorld().m_physics_debug) Lil::GetWorld().DebugDraw();
            if (selected) {
                Transform t = selected->GetTransform();
                DrawGizmo3D(GIZMO_ALL, &t);
                selected->SetTransform(t);
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                auto pick = Lil::GetWorld().PickActor(GetMousePosition(), render_target.texture.width, render_target.texture.height, m_camera);
                if (pick) selected = pick;
            }
        EndMode3D();
    EndTextureMode();
}
