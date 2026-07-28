#include "LilEngine.hpp"

Lil::Engine &Lil::Engine::Get() {
    static Lil::Engine instance;
    return instance;
}

void Lil::Engine::Init() {
    Physics().Init();
}
