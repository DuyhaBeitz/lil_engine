#pragma once

#include "CommonIncludes.hpp"
#include "Serialization.hpp"
#include "ResourceManager.hpp"
#include <vector>

#include "Actor.hpp"

#include "Character.hpp"
#include "Heightmap.hpp"
#include "Vehicle.hpp"
#include "Components/LightComponent.hpp"
#include "Components/AnimatedModelComponent.hpp"

class World {
private:
    std::unordered_map<uuids::uuid, std::unique_ptr<Actor>> m_actors;
    std::unordered_map<uuids::uuid, std::unique_ptr<Component>> m_components;
    bool m_update_ready = false;

    void PrepareUpdate() {
        UpdateActorLayout();
    }

public:
    World() = default;
    void Clear();

    template<typename T = Actor, typename... Args>
    T* CreateActor(Args&&... args) {
        static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");
        
        auto actor = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = actor.get();
        m_actors[ptr->GetID()] = std::move(actor);
        ptr->SetupComponents();
        return ptr;
    }

    Actor* CreateActor(const TypeInfo* ti) {
        if (ti->IsA<Actor>()) {
            void* p = ti->Create();
            if (p) {
                auto actor = std::unique_ptr<Actor>(static_cast<Actor*>(p));
                Actor* ptr = actor.get();
                m_actors[ptr->GetID()] = std::move(actor);
                ptr->SetupComponents();
                return ptr;
            }
            else {
                LIL_LOG_ERROR("Failed to create Actor");
                return nullptr;
            }
        }
        else {
            LIL_LOG_ERROR("Trying to create Actor type doesn't inherit from it");
            return nullptr;
        }
    }
    Actor* CopyActor(uuids::uuid original_id);
    
    Actor* GetActor(uuids::uuid id) {
        if (m_actors.find(id) == m_actors.end()) return nullptr;
        return m_actors.at(id).get();
    }

    std::unordered_map<uuids::uuid, std::unique_ptr<Actor>>& Actors();
    void DestroyActor(uuids::uuid id);
    void DestroyActor(Actor* actor);
    void DestroyAllActors();
    bool IsActorAlive(Actor* actor) const;

    
    template<typename T = Component, typename... Args>
    T* CreateComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        m_components[ptr->GetID()] = std::move(component);
        return ptr;
    }

    Component* CreateComponent(const TypeInfo* ti) {
        if (ti->IsA<Component>()) {
            void* p = ti->Create();
            if (p) {
                auto component = std::unique_ptr<Component>(static_cast<Component*>(p));
                Component* ptr = component.get();
                m_components[ptr->GetID()] = std::move(component);
                return ptr;
            }
            else {
                LIL_LOG_ERROR("Failed to create Component");
                return nullptr;
            }
        }
        else {
            LIL_LOG_ERROR("Trying to create Component type doesn't inherit from it");
            return nullptr;
        }
    }
    Component* CopyComponent(uuids::uuid original_id);

    Component* GetComponent(uuids::uuid id) {
        if (m_components.find(id) == m_components.end()) return nullptr;
        return m_components.at(id).get();
    }

    Actor* PickActor(Vector2 screen_pos, int render_w, int render_h, Camera camera);

    std::unordered_map<uuids::uuid, std::unique_ptr<Component>>& Components();
    void DestroyComponent(uuids::uuid id);
    void DestroyComponent(Component* component);
    void DestroyAllComponents();
    bool IsComponentAlive(Component* component) const;

    void Draw();
    void Update();
    void DebugDraw();
    void ToggleSimulationGoing() { m_simulation_going = !m_simulation_going; }
    bool IsSumulationGoing() { return m_simulation_going; }
    void SetSimulationGoing(bool going) { m_simulation_going = going; }

    void UpdateActorLayout();

    bool m_simulation_going = false;
    float m_simulation_speed = 1.0f;

    template <class Archive>
    void save( Archive & ar ) const {
        LIL_SER_FIELD(m_simulation_going);
        LIL_SER_FIELD(m_simulation_speed);
        LIL_SER_FIELD(m_components);
        LIL_SER_FIELD(m_actors);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        Clear();
        LIL_SER_FIELD(m_simulation_going);
        LIL_SER_FIELD(m_simulation_speed);
        LIL_SER_FIELD(m_components);
        LIL_SER_FIELD(m_actors);
    }
};
LIL_DISAMBIGUATE_LOAD_SAVE(World)