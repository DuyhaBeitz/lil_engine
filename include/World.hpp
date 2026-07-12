#pragma once

#include "CommonIncludes.hpp"
#include "ResourceManager.hpp"
#include <vector>

#include "Actor.hpp"

// #include "Pawn.hpp"
// #include "Character.hpp"
#include "Heightmap.hpp"

class World {
private:
    std::unordered_map<Actor*, std::unique_ptr<Actor>> m_actors;
    std::unordered_map<Component*, std::unique_ptr<Component>> m_components;

public:
    World() = default;

    //std::shared_ptr<Actor> AddActor(std::shared_ptr<Actor> actor);

    template<typename T = Actor, typename... Args>
    T* CreateActor(Args&&... args) {
        static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");
        
        auto actor = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = actor.get();
        m_actors[ptr] = std::move(actor);
        return ptr;
    }

    void DestroyActor(Actor* actor);
    bool IsActorAlive(Actor* actor) const;

    
    template<typename T = Component, typename... Args>
    T* CreateComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        m_components[ptr] = std::move(component);
        return ptr;
    }

    void DestroyComponent(Component* component);
    bool IsComponentAlive(Component* component) const;

    void Draw();
    void Update();
    void DebugDraw();
    void ToggleSimulationGoing() { m_simulation_going = !m_simulation_going; }

    bool m_simulation_going = false;
    float m_simulation_speed = 1.0f;
    bool m_physics_debug = false;
};