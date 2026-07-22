#pragma once

#include "CommonIncludes.hpp"
#include "ResourceManager.hpp"
#include <vector>

#include "Actor.hpp"

// #include "Pawn.hpp"
// #include "Character.hpp"
#include "Heightmap.hpp"

enum class RenderMode : uint8_t {
    Unlit = 0,
    Wireframe
};

class World {
private:
    std::unordered_map<uuids::uuid, std::unique_ptr<Actor>> m_actors;
    std::unordered_map<uuids::uuid, std::unique_ptr<Component>> m_components;
    bool m_update_ready = false;

    void UpdateActorLayout() {
        for (auto& [key, actor] : m_actors) actor->LayoutUpdate();
    }

    void PrepareUpdate() {
        UpdateActorLayout();
    }

public:
    World() = default;

    template<typename T = Actor, typename... Args>
    T* CreateActor(Args&&... args) {
        static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");
        
        auto actor = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = actor.get();
        m_actors[ptr->GetID()] = std::move(actor);
        return ptr;
    }

    Actor* CreateActor(const TypeInfo* ti) {
        if (ti->IsA<Actor>()) {
            void* p = ti->Create();
            if (p) {
                auto actor = std::unique_ptr<Actor>(static_cast<Actor*>(p));
                Actor* ptr = actor.get();
                m_actors[ptr->GetID()] = std::move(actor);
                return ptr;
            }
            else {
                LOG_ERROR("Failed to create Actor");
                return nullptr;
            }
        }
        else {
            LOG_ERROR("Trying to create Actor type doesn't inherit from it");
            return nullptr;
        }
    }

    void DestroyActor(Actor* actor);
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
                LOG_ERROR("Failed to create Component");
                return nullptr;
            }
        }
        else {
            LOG_ERROR("Trying to create Component type doesn't inherit from it");
            return nullptr;
        }
    }

    Actor* PickActor(Vector2 screen_pos, int render_w, int render_h, Camera camera);

    void DestroyComponent(Component* component);
    bool IsComponentAlive(Component* component) const;

    void Draw();
    void Update();
    void DebugDraw();
    void ToggleSimulationGoing() { m_simulation_going = !m_simulation_going; }

    RenderMode GetRenderMode() { return m_render_mode; }
    void SetRenderMode(RenderMode render_mode) { m_render_mode = render_mode; }

    bool m_simulation_going = true;
    float m_simulation_speed = 1.0f;
    bool m_physics_debug = false;
    RenderMode m_render_mode = RenderMode::Unlit;
};