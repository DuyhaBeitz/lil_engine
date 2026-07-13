#pragma once

#include "Component.hpp"
#include <vector>

class Actor : public Transformable {
public:
    std::vector<Component*> m_components;
    std::vector<Attachment> m_attachments;

public:
    LIL_REFLECTABLE()

    Actor() = default;
    
    void LayoutUpdate() {
        for (auto& attachment : m_attachments) {
            attachment.child->LayoutUpdate(attachment.parent->GetTransform());
        }
    }

    void SimulationUpdate() {
        for (auto& component : m_components) {
            component->SimulationUpdate(*this);
        }
    }

    void Draw() {
        for (auto& component : m_components) component->Draw();
    }

    void DebugDraw() {
        for (auto& component : m_components) component->DebugDraw();

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

    void AttachComponent(Component* component, Transformable* parent);
    void AttachComponent(Component* component) { AttachComponent(component, this); }
};
REFL_AUTO(type(Actor, bases<Transformable>))