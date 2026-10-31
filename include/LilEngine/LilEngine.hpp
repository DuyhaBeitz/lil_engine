#pragma once

#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "Assets.hpp"
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
        ~Engine() {
            m_world.Clear();
        }
        
        void Init();
        void Update(float delta_time, float physics_step_time);

        ::PhysicsSystem& GetPhysics();
        ::World& GetWorld();
        ::SceneManager& GetSceneManager();
        ::Audio& GetAudio();
        ::Environment& GetEnvironment();
        
    private:
        ::PhysicsSystem m_physics;
        ::World m_world;
        ::SceneManager m_scene_manager;
        ::Audio m_audio;
        ::Environment m_environment;
    };
    
    // Convenience functions
    ::PhysicsSystem& Physics();
    ::World& World();
    ::SceneManager& SceneManager();
    ::Audio& Audio();
    ::Environment& Environment();
}