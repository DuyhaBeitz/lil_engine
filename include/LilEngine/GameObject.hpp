#pragma once

#include "Reflection.hpp"
#include "Serialization.hpp"

#include "stduuid/uuid.h"
#include <random>
#include <cstdint>

#define TRANSFORM_EMPTY Transform{.translation = Vector3{0.0f, 0.0f, 0.0f}, .rotation = QuaternionIdentity(), .scale = Vector3{1.0f, 1.0f, 1.0f}}

class Transformable : public Reflectable {
public:
    Transform m_transform = TRANSFORM_EMPTY;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

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
LIL_SERIALIZE_NO_BASE(Transformable, m_transform)

class Identifiable : public Reflectable {
private:
    uuids::uuid m_id;

    static uuids::uuid GenerateID() {
        std::random_device rd;
        std::mt19937 engine(rd());
       
        uuids::uuid_random_generator gen{engine};
        return gen();
    }

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Identifiable() : m_id(GenerateID()) {}

    const uuids::uuid& GetID() const {return m_id;}
    std::string GetIDString() const {return uuids::to_string(m_id);}
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