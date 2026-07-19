#pragma once

#include "Component.hpp"
#include <vector>

class Actor : public GameObject {
public:
    // key = component; value = parent ptr
    std::unordered_map<Component*, GameObject*> m_components;
    std::set<Component*> m_marked_deattached;

public:
    LIL_REFLECTABLE()

    Actor() = default;
    
    void LayoutUpdate() ;
    void SimulationUpdate();

    void Draw();
    void DebugDraw();

    void AttachComponent(Component* component, GameObject* parent);
    void AttachComponent(Component* component);

    void DeattachComponent(Component* component);

    bool IsComponentAttached(Component* component);

    const std::unordered_map<Component*, GameObject*>& Components() const {return m_components;}
};
LIL_REFLECT(Actor, bases<GameObject>)