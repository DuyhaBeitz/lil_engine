#include "SceneManager.hpp"
#include "Serialization.hpp"
#include "LilEngine.hpp"
#include "utils/FileHelper.hpp"

void SceneManager::UpdateScene() {
    Lil::World().UpdateActorLayout();
    Lil::Assets::ApplyAllModelSettings();
    Lil::Environment().Update();
}

void SceneManager::ClearScene() {
    Lil::Environment().Reset();
    Lil::World() = World();
    UpdateScene();
}

void SceneManager::LoadScene(std::string filename)
{
    std::string content = ReadFile(filename);

    std::istringstream is(content);
    ArchiveIn ar(is);

    Lil::Assets::load_assets(ar);

    LIL_SER_FIELD(Lil::World())
    LIL_SER_FIELD(Lil::Environment())

    UpdateScene();
}

void SceneManager::SaveScene(std::string filename) {
    std::ofstream os(filename);
    ArchiveOut ar(os);
    Lil::Assets::save_assets(ar);
    LIL_SER_FIELD(Lil::World())
    LIL_SER_FIELD(Lil::Environment())
}
