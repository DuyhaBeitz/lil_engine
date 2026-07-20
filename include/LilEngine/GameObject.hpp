#pragma once

#include "Reflection.hpp"

#define TRANSFORM_EMPTY Transform{.translation = Vector3{0.0f, 0.0f, 0.0f}, .rotation = QuaternionIdentity(), .scale = Vector3{1.0f, 1.0f, 1.0f}}

class Transformable : public Reflectable {
public:
    Transform m_transform = TRANSFORM_EMPTY;

public:
    LIL_REFLECTABLE()

    Transformable(Transform transform = TRANSFORM_EMPTY) : m_transform(transform), Reflectable() {}

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
LIL_REFLECT(Transformable, bases<>,
    field(m_transform)
)

class GameObject : public Transformable {
public:
    const uint32_t m_id;

public:
    GameObject(Transform transform = TRANSFORM_EMPTY);

    virtual void Clean() {};

    LIL_REFLECTABLE()
};
LIL_REFLECT(GameObject, bases<Transformable>,
    field(m_id)
)