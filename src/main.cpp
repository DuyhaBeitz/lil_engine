// The game code
// includes LilEngine, and optionally LilEditor

#include "LilEngine.hpp"
#include "LilEditor.hpp"

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

    Lil::Engine().Get().Init();

    Lil::Editor::Get().Init();

    bool editor = true;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_I)) editor = !editor;
        if (editor) {
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
                DrawFPS(100, 100);
            EndDrawing();
        }      
    }

    Lil::Resources().Unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}