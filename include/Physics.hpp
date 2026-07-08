#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>

class Physics {
private:
    Physics() : m_world(m_physicsCommon.createPhysicsWorld()) {};
    ~Physics() = default;

    rc::PhysicsCommon m_physicsCommon;
    rc::PhysicsWorld* m_world = nullptr;

public:
    static Physics& Get() {
        static Physics instance;
        return instance;
    }
    
    rc::PhysicsCommon& GetCommon() { return m_physicsCommon; }
    rc::PhysicsWorld* GetWorld() { return m_world; }
};