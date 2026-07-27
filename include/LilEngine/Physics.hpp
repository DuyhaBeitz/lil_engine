#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>

class PhysicsSystem {
private:
    rc::PhysicsCommon m_physicsCommon;
    rc::PhysicsWorld* m_world = nullptr;

public:
    PhysicsSystem() : m_world(m_physicsCommon.createPhysicsWorld()) {};
    ~PhysicsSystem() = default;
       
    rc::PhysicsCommon& GetCommon() { return m_physicsCommon; }
    rc::PhysicsWorld* GetWorld() { return m_world; }
    void UpdateDebug();
};