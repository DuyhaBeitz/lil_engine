#include "App.hpp"
#include "Log.hpp"

#include "utils/ColliderHelper.hpp"

bool App::Init() {
    InitWindow(720, 720, m_title);
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
    ResourceManager::Get().AddTexture("assets/heightmap.png");
    ResourceManager::Get().AddModel("assets/Car.glb");
    ResourceManager::Get().AddModel("assets/Earth.glb");
    ResourceManager::Get().AddModel("assets/CesiumMan.glb");
}

void App::InitCamera() {
    m_camera.position = (Vector3){ 10.0f,10.0f, 10.0f };  // Camera position
    m_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 60.0f;                                // Camera field-of-view Y
    m_camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
}

void App::InitPhyiscs() {
    Model man_model = *ResourceManager::Get().GetModel("CesiumMan.glb");
    auto man = std::make_shared<Character>();
    man->SetModel("CesiumMan.glb");
    AddBoundsBoxColliderToBodyMulty(man->GetBody(), man_model);
    man->SetPosition({13, 5, 0});
    m_world.AddActor(man);

    auto car = m_world.AddPawn("Car.glb");
    car->AddBoundBoxColliderMulty();
    car->SetPosition({0, 5, 0});

    auto earth = m_world.AddPawn("Earth.glb");
    earth->AddBoundSphereCollider();
    earth->SetPosition({0, 20, 0});

    Vector3 map_size = {128, 4, 128};
    auto map = m_world.AddHeightmap("heightmap.png", map_size);
}

void App::Update() {
    if (!m_cursor_enabled) {UpdateCamera(&m_camera, CAMERA_FREE);}
    m_world.Update();

    if (IsKeyPressed(KEY_L)) {
        if (m_cursor_enabled) DisableCursor();
        else EnableCursor();
        m_cursor_enabled = !m_cursor_enabled;
    }
    if (IsKeyPressed(KEY_F)) {
        m_world.ToggleSimulationGoing();
    }
    if (IsKeyPressed(KEY_V)) {
        m_world.m_physics_debug = !m_world.m_physics_debug;
    }

    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }
}

void App::Draw() {
    BeginDrawing();
        ClearBackground(RAYBLACK);
        BeginMode3D(m_camera);
            m_world.Draw();
            if (m_world.m_physics_debug) m_world.DebugDraw();
        EndMode3D();
    EndDrawing();
}