#include "App.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static void Update() {
    App::Get().Update();
    App::Get().Draw();
}

int main() {
    if (!App::Get().Init()) {
        LOG_ERROR("Failed to initialize, quitting");
        return EXIT_FAILURE;
    }

#ifdef __EMSCRIPTEN__
    // Set the main loop. The second parameter (0) lets the browser choose the frame rate.
    // The third parameter (1) indicates that the loop is a simulation callback (not a render callback).
    emscripten_set_main_loop(Update, 0, 1);
#else
    while (!WindowShouldClose()) Update();
#endif

    return EXIT_SUCCESS;
}