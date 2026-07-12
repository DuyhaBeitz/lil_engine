#include "App.hpp"
#include "Log.hpp"
#include "raygizmo.h"

#include "utils/ColliderHelper.hpp"

#include "Components/ModelComponent.hpp"
#include "Components/ColliderComponent.hpp"

bool App::Init() {
    InitWindow(720, 720, m_title);
    SetTargetFPS(10);
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

Actor* car = nullptr;

void App::InitPhyiscs() {
    {
    car = Lil::GetWorld().CreateActor<Actor>();

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
}

void App::Draw() {
    BeginDrawing();
        ClearBackground(RAYBLACK);
        BeginMode3D(m_camera);
            Lil::GetWorld().Draw();
            if (Lil::GetWorld().m_physics_debug) Lil::GetWorld().DebugDraw();
            Transform t = car->GetTransform();
            DrawGizmo3D(GIZMO_ALL, &t);
            car->SetTransform(t);
        EndMode3D();
    EndDrawing();
}