#pragma once

#include "CommonIncludes.hpp"
#include "ResourceManager.hpp"
#include <vector>

#include "Actor.hpp"

class World {
public:
    World() : m_world(m_physicsCommon.createPhysicsWorld()) {}

    std::shared_ptr<Actor> AddActor(rc::BodyType bodyType = rc::BodyType::DYNAMIC);

    std::shared_ptr<Actor> AddHeightmapActor(Image heightmap_image, Vector3 map_size);
    std::shared_ptr<Actor> AddHeightmapActor(Texture2D heightmap_texture, Vector3 map_size);
    
    void Draw();
    void Update();

    void DebugDraw();

    rc::PhysicsCommon& GetPhysicsCommon() { return m_physicsCommon; }
    rc::PhysicsWorld* GetPhysicsWorld() { return m_world; }

    void ToggleSimulationGoing() { m_simulation_going = !m_simulation_going; }

    bool m_simulation_going = false;
    float m_simulation_speed = 1.f;
    bool m_physics_debug = false;

private:
    rc::PhysicsCommon m_physicsCommon;
    rc::PhysicsWorld* m_world;
    std::unordered_map<std::string, std::shared_ptr<Actor>> m_actors;
};