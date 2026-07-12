#pragma once

#include "Component.hpp"
#include <vector>

class Actor : public Transformable {
private:
    std::vector<Component*> m_components;
    std::vector<Attachment> m_attachments;

    void PropogateTransform() {
        for (auto& attachment : m_attachments) {
            attachment.child->PropogateTransform(attachment.parent->GetTransform());
        }
    }

public:
    Actor() = default;
    void LayoutUpdate() {
        PropogateTransform();
    }

    void ComponentsUpdate() {
        for (auto& component : m_components) {
            component->Update(*this);
        }
        CustomUpdate();
    }

    // for editing scene, attaching components (actor propogates to components)
    virtual void CustomUpdate() {};

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