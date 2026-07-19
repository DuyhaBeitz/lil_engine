#include "Actor.hpp"

void Actor::LayoutUpdate(){

    for (auto& component : m_marked_deattached) {
        m_components.erase(component);   
    }
    m_marked_deattached.clear();    

    for (auto& [component, parent] : m_components) {
        component->LayoutUpdate(parent->GetTransform());
    }

}

void Actor::SimulationUpdate(){
    for (auto& [component, parent] : m_components) {
        if (!IsComponentAttached(component)) continue;
        component->SimulationUpdate(*this);
    }
}

void Actor::Draw() {
    for (auto& [component, parent] : m_components) {
        if (!IsComponentAttached(component)) continue;
        component->Draw();
    }
}

void Actor::DebugDraw() {
    for (auto& [component, parent] : m_components) {
        if (!IsComponentAttached(component)) continue;
        component->DebugDraw();
    }

    Vector3 v[3] = {
        Vector3{1, 0, 0},
        Vector3{0, 1, 0},
        Vector3{0, 0, 1}
    };
    Color c[3] = {RED, GREEN, BLUE};
    for (int i = 0; i < 3; i++) {
        DrawLine3D(GetPosition(), GetPosition()+Vector3RotateByQuaternion(v[i], GetRotation())*10.0f, c[i]);
    }
};

void Actor::AttachComponent(Component *component, GameObject *parent) {
    m_components[component] = parent;
}

void Actor::AttachComponent(Component *component) {
    AttachComponent(component, this);
}

void Actor::DeattachComponent(Component *component) {
    if (!component) return;
    auto it = m_components.find(component);
    if (it != m_components.end()) m_marked_deattached.insert(component);
}


bool Actor::IsComponentAttached(Component *component) {
    bool exists = (m_components.find(component) != m_components.end());
    bool not_deattached = (m_marked_deattached.find(component) == m_marked_deattached.end());
    return exists && not_deattached;
}
