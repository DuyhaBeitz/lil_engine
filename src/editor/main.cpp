#include "App.hpp"

int main() {
    if (!App::Get().Init()) {
        LOG_ERROR("Failed to initialize, quitting");
        return EXIT_FAILURE;
    }

    while (!WindowShouldClose()) {
        App::Get().Update();
        App::Get().Draw();
    }

    return EXIT_SUCCESS;
}