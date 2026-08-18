#pragma once

#include "Reflection.hpp"
#include "Serialization.hpp"

#include "stduuid/uuid.h"
#include <random>
#include <cstdint>

#define TRANSFORM_EMPTY Transform{.translation = Vector3{0.0f, 0.0f, 0.0f}, .rotation = Quaternion{0.0f, 0.0f, 0.0f, 1.0f}, .scale = Vector3{1.0f, 1.0f, 1.0f}}

class Transformable : public Reflectable {
public:
    Transform m_transform = TRANSFORM_EMPTY;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Transformable(Transform transform = TRANSFORM_EMPTY);

    void SetTransform(Transform transform);
    void SetPosition(Vector3 position);
    void SetRotation(Quaternion rotation);
    void SetScale(Vector3 scale);

    Transform GetTransform() const;
    Vector3 GetPosition() const;
    Quaternion GetRotation() const;
    Vector3 GetScale() const;

    Vector3 GetAxisAngle(float& angle) const;
};
LIL_REFLECT(Transformable, bases<>,
    field(m_transform)
)
LIL_SERIALIZE_NO_BASE(Transformable, m_transform)

class Identifiable : public Reflectable {
private:
    uuids::uuid m_id;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Identifiable();

    const uuids::uuid& GetID() const;
    std::string GetIDString() const;
    void SetID(uuids::uuid id);

    static uuids::uuid GenerateID();
};
LIL_REFLECT(Identifiable, bases<>)
LIL_SERIALIZE_NO_BASE(Identifiable, m_id)

class GameObject : public Identifiable, public Transformable {
public:
    GameObject(Transform transform = TRANSFORM_EMPTY);
    virtual ~GameObject() = default;

    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
};
LIL_REFLECT(GameObject, (bases<Identifiable, Transformable>))

LIL_SER_BEGIN(GameObject)
LIL_SER_BASE(Identifiable)
LIL_SER_BASE(Transformable)
LIL_SER_END()