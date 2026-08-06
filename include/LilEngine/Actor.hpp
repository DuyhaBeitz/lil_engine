#pragma once

#include "Component.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ModelComponent.hpp"
#include <vector>

class Actor : public GameObject {
public:
    std::vector<Component*> m_components;
    std::set<const Component*> m_marked_deattached;
    virtual void OnLayoutUpdate() {};
    virtual void OnSimulationUpdate(float delta_time) {};

public:
    LIL_REFLECTABLE()

    Actor() = default;
    
    virtual void SetupComponents() {}

    void LayoutUpdate();
    void SimulationUpdate(float delta_time);

    void Draw();

    void DebugUpdate();
    void DebugDraw();

    void AttachComponent(Component* component);

    void DeattachComponent(Component* component);

    bool IsComponentAttached(const Component* component) const;

    const std::vector<Component*>& Components() const {return m_components;}

    void AttachComponentFromId(uuids::uuid id);

    template <class Archive>
    void save( Archive & ar ) const {
        LIL_SAVE_BASE(GameObject)
        std::vector<uuids::uuid> component_ids = {};
        for (Component* component : m_components) {
            if (!IsComponentAttached(component)) continue;
            component_ids.push_back(component->GetID());
        }
        ar(component_ids);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        LIL_LOAD_BASE(GameObject)
        std::vector<uuids::uuid> component_ids = {};
        ar(component_ids);
        for (uuids::uuid id : component_ids) {
            AttachComponentFromId(id);
        }        
    }

    template <class Comp>
    Comp* GetFirst() {
        static_assert(std::is_base_of_v<Component, Comp>, "Comp must be derived from Component");
        for (auto& component : m_components) {
            if (Comp* c = dynamic_cast<Comp*>(component)) return c;
        }
        return nullptr;
    }
};
LIL_REFLECT(Actor, bases<GameObject>)
LIL_DISAMBIGUATE_LOAD_SAVE(Actor)