#include "World.hpp"
#include "utils/ColliderHelper.hpp"
#include "LilEngine.hpp"

#include "Components/ModelComponent.hpp"

// std::shared_ptr<Actor> World::AddActor(std::shared_ptr<Actor> actor) {
//     m_actors[std::to_string(m_actors.size())] = actor;
//     return actor;
// }

// std::shared_ptr<Pawn> World::AddPawn(std::string model_key, Vector3 model_offset) {
//     auto pawn = std::make_shared<Pawn>();
//     pawn->SetModel(model_key, model_offset);
//     AddActor(pawn);
//     return pawn;
// }

// std::shared_ptr<Heightmap> World::AddHeightmap(Image heightmap_image, Vector3 map_size) {
//     auto heightmap = std::make_shared<Heightmap>(heightmap_image, map_size);
//     AddActor(heightmap);
//     return heightmap;
// }

// std::shared_ptr<Heightmap> World::AddHeightmap(Texture2D heightmap_texture, Vector3 map_size) {
//     auto heightmap = std::make_shared<Heightmap>(heightmap_texture, map_size);
//     AddActor(heightmap);
//     return heightmap;
// }

// std::shared_ptr<Heightmap> World::AddHeightmap(std::string texture_key, Vector3 map_size) {
//     auto heightmap = std::make_shared<Heightmap>(*Lil::Resources().GetTexture(texture_key), map_size);
//     AddActor(heightmap);
//     return heightmap;
// }

void World::DestroyActor(Actor *actor) {
    if (!actor) return;
    auto it = m_actors.find(actor);
    if (it != m_actors.end()) {
        m_actors.erase(it);
    }
}

bool World::IsActorAlive(Actor *actor) const {
    return actor && m_actors.find(actor) != m_actors.end();
}

Actor *World::PickActor(Vector2 screen_pos, int render_w, int render_h, Camera camera) {
    Ray ray = GetScreenToWorldRayEx(screen_pos, camera, render_w, render_h);

    float closest = INFINITY;
    Actor* a = nullptr;
    
    DrawRay(ray, RED);
    for (auto& [key, actor] : m_actors) {
        for (auto& component : actor->Components()) {
            if (ModelComponent* m = dynamic_cast<ModelComponent*>(component)) {            
                RayCollision res = m->Raycast(ray);
                if (res.hit && res.distance < closest) {
                    closest = res.distance;
                    a = actor.get();
                }
            }
        }
    }
    return a;
}

void World::DestroyComponent(Component *component)
{
    if (!component) return;
    auto it = m_components.find(component);
    if (it != m_components.end()) {
        m_components.erase(it);
    }
}

bool World::IsComponentAlive(Component *component) const {
    return component && m_components.find(component) != m_components.end();
}

void World::Draw() {
    for (auto& [key, actor] : m_actors) actor->Draw();
}

void World::Update() {
    if (!m_update_ready) {
        PrepareUpdate();
        m_update_ready = true;
    }

    float time_step = GetFrameTime()*m_simulation_speed;
    if (m_simulation_going && time_step > 0.0) Lil::Physics().GetWorld()->update(time_step);
    if (m_simulation_going) for (auto& [key, actor] : m_actors) actor->SimulationUpdate();
    
    UpdateActorLayout();

    // IF YOU WANT TO DRAW DEBUG WHILE PAUSING THE SIMULATION, SET TIMESTEP TO 0 (INSTEAD OF NOT CALLING UPDATE AT ALL)
    // WITHOUT UPDATING THE PHYSICS WORLD DEBUG DRAW IS VERY SLOW

    // ACTUALLY, PROVIDING 0 IS ILLEGAL HERE, SO
    // TODO: FIX PHYSICS DEBUG SLOW WHEN NO UPDATING
}

void World::DebugDraw(){
    for (auto& [key, actor] : m_actors) actor->DebugDraw();
}
