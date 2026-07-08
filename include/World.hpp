#pragma once

#include "CommonIncludes.hpp"
#include "ResourceManager.hpp"
#include <vector>

#include "Actor.hpp"
#include "Pawn.hpp"
#include "Heightmap.hpp"

class World {
public:
    World() = default;

    std::shared_ptr<Actor> AddActor(std::shared_ptr<Actor> actor);
    
    void Draw();
    void Update();

    void DebugDraw();

    void ToggleSimulationGoing() { m_simulation_going = !m_simulation_going; }

    bool m_simulation_going = false;
    float m_simulation_speed = 1.0f;
    bool m_physics_debug = false;

private:
    std::unordered_map<std::string, std::shared_ptr<Actor>> m_actors;
};