#include "Actor.hpp"
#include <algorithm>
#include "LilEngine.hpp"

void Actor::LayoutUpdate(){
    for (auto& component : m_marked_deattached) {
        std::erase_if(m_components, [this](Component* comp) {
            return m_marked_deattached.contains(comp);
        });
    }
    m_marked_deattached.clear();    

    for (Component* component : m_components) {
        component->LayoutUpdate(GetTransform());
    }
    OnLayoutUpdate();
}

void Actor::SimulationUpdate(){
    for (auto& component : m_components) {
        if (!IsComponentAttached(component)) continue;
        component->SimulationUpdate(*this);
    }
}

void Actor::Draw() {
    for (auto& component : m_components) {
        if (!IsComponentAttached(component)) continue;
        component->Draw();
    }
}

void Actor::DebugDraw() {
    for (auto& component : m_components) {
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

void Actor::AttachComponent(Component *component) {
    if (!IsComponentAttached(component)) {
        m_components.push_back(component);
    }    
}

void Actor::DeattachComponent(Component *component) {
    if (!component) return;
    auto it = std::find(m_components.begin(), m_components.end(), component);
    if (it != m_components.end()) m_marked_deattached.insert(component);
}


bool Actor::IsComponentAttached(const Component *component) const {
    bool exists = std::find(m_components.begin(), m_components.end(), component) != m_components.end();
    bool not_deattached = (m_marked_deattached.find(component) == m_marked_deattached.end());
    return component && exists && not_deattached;
}

void Actor::CreateComponentFromId(uuids::uuid id) {
    if (Component* component = Lil::GetWorld().GetComponent(id)) {
        m_components.push_back(component);
    }
}
