#pragma once

#include "CommonIncludes.hpp"
#include "ResourceManager.hpp"
#include <vector>

#include "Actor.hpp"

#include "Pawn.hpp"
#include "Character.hpp"
#include "Heightmap.hpp"

class World {
public:
    World() = default;

    std::shared_ptr<Actor> AddActor(std::shared_ptr<Actor> actor);
    std::shared_ptr<Pawn> AddPawn(std::string model_key, Vector3 model_offset = Vector3{0.0f, 0.0f, 0.0f});
    
    std::shared_ptr<Heightmap> AddHeightmap(Image heightmap_image, Vector3 map_size);
    std::shared_ptr<Heightmap> AddHeightmap(Texture2D heightmap_texture, Vector3 map_size);
    std::shared_ptr<Heightmap> AddHeightmap(std::string texture_key, Vector3 map_size);


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