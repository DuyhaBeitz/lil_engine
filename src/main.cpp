// The game code
// includes LilEngine, and optionally LilEditor

#include "Character.hpp"
#include "Heightmap.hpp"
#include "Vehicle.hpp"
#include "Components/LightComponent.hpp"
#include "Components/AnimatedModelComponent.hpp"
#include "Components/InstancedModelComponent.hpp"

#include "LilEngine.hpp"
#include "LilEditor.hpp"

constexpr int target_fps = 120;
constexpr float dt = 1.0f / float(target_fps);

static Camera camera = {
    .position = (Vector3){ 10.0f,10.0f, 10.0f },  // Camera position
    .target = (Vector3){ 0.0f, 0.0f, 0.0f },      // Camera looking at point
    .up = (Vector3){ 0.0f, 1.0f, 0.0f },          // Camera up vector (rotation towards target)
    .fovy = 60.0f,                                // Camera field-of-view Y
    .projection = CAMERA_PERSPECTIVE              // Camera mode type
};

void DrawGame() {
    R3D_Begin(camera);
    Lil::World().Draw();
    R3D_End();
}

int main() {
    InitWindow(1920, 1080, "game");
    R3D_Init(GetScreenWidth(), GetScreenHeight());
    InitAudioDevice();
    SetTraceLogLevel(LOG_ALL);
    DisableCursor();
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(target_fps);

    Lil::Engine().Get().Init();

    Lil::Editor::Get().Init();

    bool editor = true;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_I)) editor = !editor;
        if (editor) {
            Lil::Editor::Get().UpdateFixedAccumulative(dt);
            BeginDrawing();
            Lil::Editor::Get().Draw();
            EndDrawing();
        }
        else {
            UpdateCamera(&camera, CAMERA_FREE);
            Lil::Engine::Get().UpdateFixedAccumulative(dt);

            BeginDrawing();
                ClearBackground(RAYBLACK);
                DrawGame();
                DrawFPS(100, 100);
            EndDrawing();
        }      
    }

    Lil::Editor::Get().Close();
    Lil::Resources().Unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}