#pragma once

#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "ResourceManager.hpp"
#include "Audio.hpp"
#include "SceneManager.hpp"
#include "Environment.hpp"
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

        ::PhysicsSystem& GetPhysics();
        ::ResourceManager& GetResources();
        ::World& GetWorld();
        ::SceneManager& GetSceneManager();
        ::Audio& GetAudio();
        ::Environment& GetEnvironment();
        
    private:
        ::PhysicsSystem m_physics;
        ::ResourceManager m_resources;
        ::World m_world;
        ::SceneManager m_scene_manager;
        ::Audio m_audio;
        ::Environment m_environment;
    };
    
    // Convenience functions
    ::PhysicsSystem& Physics();
    ::ResourceManager& Resources();
    ::World& World();
    ::SceneManager& SceneManager();
    ::Audio& Audio();
    ::Environment& Environment();
}