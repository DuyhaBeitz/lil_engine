#pragma once

#include "Reflection.hpp"

class Transformable : Reflectable {
public:
    Transform m_transform = {
        .translation = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = QuaternionIdentity(),
        .scale = Vector3{1.0f, 1.0f, 1.0f}
    };

public:
    LIL_REFLECTABLE()

    Transformable() = default;
    Transformable(Transform transform) : m_transform(transform), Reflectable() {}

    void SetTransform(Transform transform) {m_transform = transform;}
    void SetPosition(Vector3 position) {m_transform.translation = position;}
    void SetRotation(Quaternion rotation) {m_transform.rotation = rotation;}
    void SetScale(Vector3 scale) {m_transform.scale = scale;}

    Transform GetTransform() const {return m_transform;}
    Vector3 GetPosition() const {return m_transform.translation;}
    Quaternion GetRotation() const {return m_transform.rotation;}
    Vector3 GetScale() const {return m_transform.scale;}

    Vector3 GetAxisAngle(float& angle) const;
};
REFL_AUTO(
    type(Transformable),
    field(m_transform)
)

class Actor;

class Component : public Transformable {
public:
    Transformable m_local_space;

public:
    LIL_REFLECTABLE()

    Component() = default;
    Component(Transform local_transform) : m_local_space(local_transform), Transformable() {}

    void LayoutUpdate(Transform parent_transform) {
        Vector3 rotated = Vector3RotateByQuaternion(m_local_space.GetPosition(), parent_transform.rotation);
        SetPosition(parent_transform.translation + rotated);
        SetRotation(QuaternionMultiply(parent_transform.rotation, m_local_space.GetRotation()));
        SetScale(m_local_space.GetScale() * parent_transform.scale);

        OnLayoutUpdate();
    }

    Transformable& Local() { return m_local_space; }

    virtual void SimulationUpdate(Actor& actor) {};
    virtual void Draw() {};
    virtual void DebugDraw() {};

    virtual void OnLayoutUpdate() {};
};
REFL_AUTO(
    type(Component, bases<Transformable>),
    field(m_local_space)
)

struct Attachment {
    Transformable* parent;
    Component* child;
};
