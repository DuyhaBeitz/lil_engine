#pragma once

#include "Component.hpp"
#include <vector>

class Actor : public GameObject {
public:
    std::vector<Component*> m_components;
    std::set<Component*> m_marked_deattached;

public:
    LIL_REFLECTABLE()

    Actor() = default;
    
    void LayoutUpdate();
    void SimulationUpdate();

    void Draw();
    void DebugDraw();

    void AttachComponent(Component* component);

    void DeattachComponent(Component* component);

    bool IsComponentAttached(Component* component);

    const std::vector<Component*>& Components() const {return m_components;}
};

LIL_REFLECT_EX(std::vector<Component*>, bases<>, std_vector_ComponentPtr)
LIL_REFLECT(Actor, bases<GameObject>)