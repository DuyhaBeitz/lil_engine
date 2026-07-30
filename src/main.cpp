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