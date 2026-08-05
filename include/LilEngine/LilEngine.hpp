#pragma once

#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "ResourceManager.hpp"
#include "Audio.hpp"
#include "SceneManager.hpp"
#include "World.hpp"

#include "imgui.h"
#include "rlImGui.h"

#include "Log.hpp"

namespace Lil {
    class Engine {
    public:
        static Engine& Get();
        
        void Init();
        void Update();

        PhysicsSystem& GetPhysics() { return m_physics; }
        ResourceManager& GetResources() { return m_resources; }
        World& GetWorld() { return m_world; }
        SceneManager& GetSceneManager() { return m_scene_manager; }
        Audio& GetAudio() { return m_audio; }
        
    private:
        PhysicsSystem m_physics;
        ResourceManager m_resources;
        World m_world;
        SceneManager m_scene_manager;
        Audio m_audio;
    };
    
    // Convenience functions
    inline PhysicsSystem& Physics() { return Engine::Get().GetPhysics(); }
    inline ResourceManager& Resources() { return Engine::Get().GetResources(); }
    inline World& World() { return Engine::Get().GetWorld(); }
    inline SceneManager& SceneManager() { return Engine::Get().GetSceneManager(); }
    inline Audio& Audio() { return Engine::Get().GetAudio(); }
}