#include "App.hpp"
#include "Log.hpp"

bool App::Init() {
    InitWindow(500, 500, m_title);
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
}

void App::InitCamera() {
    m_camera.position = (Vector3){ 10.0f,10.0f, 10.0f };  // Camera position
    m_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 60.0f;                                // Camera field-of-view Y
    m_camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
}

void App::InitPhyiscs() {
    Model car_model = *ResourceManager::Get().GetModel("Car.glb");
    auto car = m_world.AddActor();
    car->SetModel("Car.glb");
    AddBoundsBoxColliderToBodyMulty(m_world.GetPhysicsCommon(), car->GetBody(), car_model);
    car->SetPosition({0, 5, 0});

    Model earth_model = *ResourceManager::Get().GetModel("Earth.glb");
    auto earth = m_world.AddActor();
    earth->SetModel("Earth.glb");
    AddBoundingSphereColliderToBody(m_world.GetPhysicsCommon(), earth->GetBody(), earth_model);
    earth->SetPosition({0, 20, 0});

    Vector3 map_size = {128, 4, 128};
    auto map = m_world.AddHeightmapActor(*ResourceManager::Get().GetTextrue("heightmap.png"), map_size);
}


void App::Update() {
    if (!m_cursor_enabled) {UpdateCamera(&m_camera, CAMERA_FREE);}
    m_world.Update();

    if (IsKeyPressed(KEY_F4)) {
        if (m_cursor_enabled) DisableCursor();
        else EnableCursor();
        m_cursor_enabled = !m_cursor_enabled;
    }
    if (IsKeyPressed(KEY_F5)) {
        m_world.ToggleSimulationGoing();
    }
    if (IsKeyPressed(KEY_F6)) {
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