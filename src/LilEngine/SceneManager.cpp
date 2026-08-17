#include "SceneManager.hpp"
#include "Serialization.hpp"
#include "LilEngine.hpp"

void SceneManager::LoadScene(std::string filename) {
    std::ifstream is(filename);
    ArchiveIn a_in(is);
    a_in(Lil::Resources(), Lil::Environment(), Lil::World());
    Lil::World().UpdateActorLayout();
    Lil::Resources().ApplyModelSettings();
}

void SceneManager::SaveScene(std::string filename) {
    std::ofstream os(filename);
    ArchiveOut a_out(os);
    a_out(Lil::Resources(), Lil::Environment(), Lil::World());
}
