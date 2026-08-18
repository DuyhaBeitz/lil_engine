#include "LilEngine.hpp"

namespace Lil {
        
    Engine &Engine::Get() {
        static Engine instance;
        return instance;
    }


    ::PhysicsSystem &Engine::GetPhysics()     { return m_physics; }
    ::ResourceManager& Engine::GetResources() { return m_resources; }
    ::World& Engine::GetWorld()               { return m_world; }
    ::SceneManager& Engine::GetSceneManager() { return m_scene_manager; }
    ::Audio& Engine::GetAudio()               { return m_audio; }
    ::Environment& Engine::GetEnvironment()   { return m_environment; }

    ::PhysicsSystem& Physics()     { return Engine::Get().GetPhysics(); }
    ::ResourceManager& Resources() { return Engine::Get().GetResources(); }
    ::World& World()               { return Engine::Get().GetWorld(); }
    ::SceneManager& SceneManager() { return Engine::Get().GetSceneManager(); }
    ::Audio& Audio()               { return Engine::Get().GetAudio(); }
    ::Environment& Environment()   { return Engine::Get().GetEnvironment(); }

    void Engine::Init() {
        Physics().Init();
    }

    void Engine::Update() {
        GetWorld().Update();
    }

}
