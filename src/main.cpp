// The game code
// includes LilEngine, and optionally LilEditor

#include "LilEngine.hpp"
#include "LilEditor.hpp"

#include "utils/ColliderHelper.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/ColliderComponent.hpp"

static Camera camera = {
    .position = (Vector3){ 10.0f,10.0f, 10.0f },  // Camera position
    .target = (Vector3){ 0.0f, 0.0f, 0.0f },      // Camera looking at point
    .up = (Vector3){ 0.0f, 1.0f, 0.0f },          // Camera up vector (rotation towards target)
    .fovy = 60.0f,                                // Camera field-of-view Y
    .projection = CAMERA_PERSPECTIVE              // Camera mode type
};

void DrawGame() {
    BeginMode3D(camera);
    Lil::World().Draw();
    EndMode3D();
}

int main() {
    InitWindow(1920, 1080, "game");
    SetTraceLogLevel(LOG_NONE);
    DisableCursor();

    Lil::Engine().Get().Init();

    Lil::Resources().TextureAdd("assets/heightmap.png");
    Lil::Resources().ModelAdd("assets/Car.glb");
    Lil::Resources().ModelAdd("assets/Earth.glb");
    Lil::Resources().ModelAdd("assets/CesiumMan.glb");

    {
    // auto car = Lil::World().CreateActor<Actor>();
    // auto m = Lil::World().CreateComponent<ModelComponent>("Car.glb");
    // car->AttachComponent(m);
    // auto c = Lil::World().CreateComponent<ColliderComponent>(rc::BodyType::DYNAMIC);
    // auto b = c->GetBody();
    // AddBoundsBoxColliderToBody(b, *m->GetModel());
    // car->AttachComponent(c);
    // car->SetPosition({0, 15, 0});
    
    // Vector3 map_size = {128, 4, 128};
    // auto map = Lil::World().CreateActor<Heightmap>("heightmap.png", map_size);
    // map->SetPosition(Vector3{0, -4, 0});
    }

    Lil::Editor::Get().Init();
    while (!WindowShouldClose()) {
        if (1) {
            Lil::Editor::Get().Update();
            BeginDrawing();
            Lil::Editor::Get().Draw();
            EndDrawing();
        }
        else {
            UpdateCamera(&camera, CAMERA_FREE);
            Lil::Engine::Get().Update();

            BeginDrawing();
                ClearBackground(RAYBLACK);
                DrawGame();
            EndDrawing();
        }      
    }

    Lil::Resources().Unload();
    CloseWindow();

    return 0;
}