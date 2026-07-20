#pragma once

#include "Reflection.hpp"
#include "GameObject.hpp"

class Actor;

class Component : public GameObject {
private:
    virtual void OnLayoutUpdate() {};

public:
    LIL_REFLECTABLE()
    Transform m_local_transform = TRANSFORM_EMPTY;

public:
    Component() = default;
    Component(Transform local_transform) : m_local_transform(local_transform), GameObject() {}

    void LayoutUpdate(Transform parent_transform) {
        Vector3 rotated = Vector3RotateByQuaternion(m_local_transform.translation, parent_transform.rotation);
        SetPosition(parent_transform.translation + rotated);
        SetRotation(QuaternionMultiply(parent_transform.rotation, m_local_transform.rotation));
        SetScale(m_local_transform.scale * parent_transform.scale);

        OnLayoutUpdate();
    }

    Transform& Local() { return m_local_transform; }

    virtual void SimulationUpdate(Actor& actor) {};
    virtual void Draw() {};
    virtual void DebugDraw() {};
};
LIL_REFLECT(Component, bases<GameObject>,
    field(m_local_transform)
)
