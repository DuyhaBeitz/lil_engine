#pragma once

#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "ResourceManager.hpp"
#include "World.hpp"

namespace Lil {
    class Engine {
    public:
        static Engine& Get();
        
        PhysicsSystem& GetPhysics() { return m_physics; }
        ResourceManager& GetResources() { return m_resources; }
        World& GetWorld() { return m_world; }
        
    private:
        PhysicsSystem m_physics;
        ResourceManager m_resources;
        World m_world;
    };
    
    // Convenience functions
    inline PhysicsSystem& Physics() { return Engine::Get().GetPhysics(); }
    inline ResourceManager& Resources() { return Engine::Get().GetResources(); }
    inline World& GetWorld() { return Engine::Get().GetWorld(); }
}