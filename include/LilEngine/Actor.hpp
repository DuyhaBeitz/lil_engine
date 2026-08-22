#pragma once

#include "GameObject.hpp"
#include <vector>

class Component;

class Actor : public GameObject {
public:
    std::vector<Component*> m_components;
    std::set<const Component*> m_marked_deattached;

public:
    LIL_REFLECTABLE()

    Actor() = default;
    virtual ~Actor() = default;
    
    virtual void SetupComponents() {}

    virtual void LayoutUpdate();
    virtual void SimulationUpdate(float delta_time);

    virtual void Draw();

    virtual void DebugUpdate();
    virtual void DebugDraw();

    virtual RayCollision Raycast(Ray ray) const;

    void AttachComponent(Component* component);

    void DeattachComponent(Component* component);

    bool IsComponentAttached(const Component* component) const;

    const std::vector<Component*>& Components() const;
    std::vector<uuids::uuid> GetComponentIDs() const;

    void AttachComponentFromId(uuids::uuid id);
    void AttachComponents(std::vector<uuids::uuid> component_ids);

    template <class Archive>
    void save( Archive & ar ) const {
        std::vector<uuids::uuid> component_ids = GetComponentIDs();
        LIL_SER_FIELD(component_ids);
        LIL_SAVE_BASE(GameObject);
    }
       
    template <class Archive>
    void load( Archive & ar ) {
        std::vector<uuids::uuid> component_ids = {};
        LIL_SER_FIELD(component_ids);
        LIL_LOAD_BASE(GameObject)
        AttachComponents(component_ids);
    }

    template <class Archive>
    std::vector<uuids::uuid> load_no_attaching_components( Archive & ar ) {
        std::vector<uuids::uuid> component_ids = {};
        LIL_SER_FIELD(component_ids);
        LIL_LOAD_BASE(GameObject)
        return component_ids;
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