#include "World.hpp"
#include "utils/ColliderHelper.hpp"
#include "LilEngine.hpp"

#include "Components/ModelComponent.hpp"

void World::Clear() {
    LIL_LOG_TRACE("world clear");
    DestroyAllActors();
    DestroyAllComponents();
    m_update_ready = false;
}

std::unordered_map<uuids::uuid, std::unique_ptr<Actor>> &World::Actors() {return m_actors;}

void World::DestroyActor(uuids::uuid id)
{
    LIL_LOG_TRACE("Destroying actor");
    auto it = m_actors.find(id);
    if (it != m_actors.end()) {
        m_actors.erase(it);
    }
}

void World::DestroyActor(Actor *actor) {
    if (!actor) return;
    DestroyActor(actor->GetID());
}

void World::DestroyAllActors() {
    LIL_LOG_TRACE("Destroying all actors");
    std::set<uuids::uuid> ids_to_destroy = {};
    for (auto& [id, actor] : m_actors) ids_to_destroy.insert(id);
    for (auto& id : ids_to_destroy) DestroyActor(id);
}

bool World::IsActorAlive(Actor *actor) const {
    return actor && m_actors.find(actor->GetID()) != m_actors.end();
}

Actor* World::CopyActor(uuids::uuid original_id) {
    Actor* original = GetActor(original_id);
    if (!original) return nullptr;
    
    std::unique_ptr<Actor> copy = std::make_unique<Actor>();
    std::stringstream ss;

    {
        ArchiveOut out(ss);
        original->save(out);
    }

    std::vector<uuids::uuid> original_component_ids;
    {
        ArchiveIn in(ss);
        original_component_ids = copy->load_no_attaching_components(in);
    }

    std::vector<uuids::uuid> new_component_ids;
    new_component_ids.reserve(original_component_ids.size());

    for (const auto& old_id : original_component_ids) {
        Component* new_component = CopyComponent(old_id);
        if (!new_component) {
            // abort copying, destroy all copied components
            for (const auto& new_id : new_component_ids) {
                DestroyComponent(new_id);
            }
            return nullptr;
        }
        new_component_ids.push_back(new_component->GetID());
    }
    copy->AttachComponents(new_component_ids);

    const uuids::uuid new_id = Identifiable::GenerateID();
    copy->SetID(new_id);
    Actor* result = copy.get();
    m_actors.emplace(new_id, std::move(copy));
    return result;
}

Component *World::CopyComponent(uuids::uuid original_id) {
    auto it = m_components.find(original_id);
    if (it == m_components.end()) return nullptr;

    std::stringstream ss;
    
    {
        ArchiveOut out(ss);
        out(cereal::make_nvp("component", it->second));
    }
    
    std::unique_ptr<Component> copy;
   
    {
        ArchiveIn in(ss);
        in(cereal::make_nvp("component", copy));
    }
    
    const uuids::uuid new_id = Identifiable::GenerateID();
    copy->SetID(new_id);
    Component* result = copy.get();
    m_components.emplace(new_id, std::move(copy));
    return result;
}

Actor *World::PickActor(Vector2 screen_pos, int render_w, int render_h, Camera camera)
{
    LIL_LOG_TRACE("Picking actor");
    Ray ray = GetScreenToWorldRayEx(screen_pos, camera, render_w, render_h);

    float closest = INFINITY;
    Actor* a = nullptr;
    
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

std::unordered_map<uuids::uuid, std::unique_ptr<Component>> &World::Components() {return m_components;}

void World::DestroyComponent(uuids::uuid id)
{
    LIL_LOG_TRACE("Destroying component");
    auto it = m_components.find(id);
    if (it != m_components.end()) {
        m_components.erase(it);
    }
}

void World::DestroyComponent(Component *component) {
    if (!component) return;
    DestroyComponent(component->GetID());
}

void World::DestroyAllComponents() {
    LIL_LOG_TRACE("Destroying all components");
    std::set<uuids::uuid> ids_to_destroy = {};
    for (auto& [id, component] : m_components) ids_to_destroy.insert(id);
    for (auto& id : ids_to_destroy) DestroyComponent(id);
}

bool World::IsComponentAlive(Component *component) const {
    return component && m_components.find(component->GetID()) != m_components.end();
}

void World::Draw() {
    LIL_LOG_TRACE("World drawing");
    for (auto& [key, actor] : m_actors) actor->Draw();
}

void World::Update() {
    LIL_LOG_TRACE("World updating");
    if (!m_update_ready) {
        PrepareUpdate();
        m_update_ready = true;
    }

    float time_step = GetFrameTime()*m_simulation_speed;
    if (m_simulation_going && time_step > 0.0) Lil::Physics().Step(time_step);
    if (m_simulation_going) for (auto& [key, actor] : m_actors) actor->SimulationUpdate(time_step);
    
    LIL_LOG_TRACE("World: updating actor layout");
    UpdateActorLayout();
    LIL_LOG_TRACE("World: updating actor layout DONE");

    // IF YOU WANT TO DRAW DEBUG WHILE PAUSING THE SIMULATION, SET TIMESTEP TO 0 (INSTEAD OF NOT CALLING UPDATE AT ALL)
    // WITHOUT UPDATING THE PHYSICS WORLD DEBUG DRAW IS VERY SLOW

    // ACTUALLY, PROVIDING 0 IS ILLEGAL HERE, SO
    // TODO: FIX PHYSICS DEBUG SLOW WHEN NO UPDATING
}

void World::UpdateActorLayout() {
    for (auto& [key, actor] : m_actors) actor->LayoutUpdate();
}

void World::DebugDraw(){
    LIL_LOG_TRACE("World debug drawing");

    for (auto& [key, actor] : m_actors) actor->DebugUpdate();
    for (auto& [key, actor] : m_actors) actor->DebugDraw();
}