#include "SceneManager.hpp"
#include "Serialization.hpp"
#include "LilEngine.hpp"

void SceneManager::UpdateScene() {
    Lil::World().UpdateActorLayout();
    Lil::Resources().ApplyModelSettings();
    Lil::Environment().Update();
}

void SceneManager::ClearScene() {
    Lil::Resources() = ResourceManager();
    Lil::Environment() = Environment();
    Lil::World() = World();
    UpdateScene();
}

void SceneManager::LoadScene(std::string filename) {
    std::ifstream is(filename);
    ArchiveIn a_in(is);
    a_in(Lil::Resources(), Lil::Environment(), Lil::World());
    UpdateScene();
}

void SceneManager::SaveScene(std::string filename) {
    std::ofstream os(filename);
    ArchiveOut a_out(os);
    a_out(Lil::Resources(), Lil::Environment(), Lil::World());
}
