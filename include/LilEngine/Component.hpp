#pragma once

#include "Reflection.hpp"
#include "GameObject.hpp"

class Actor;

class Component : public GameObject {
private:
    virtual void OnLayoutUpdate() {};

    // required components cannot be detached, they are native for actor that uses them
    // e.g. specifically setup model component for heightmap actor
    bool m_is_required = false;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    Transform m_local_transform = TRANSFORM_EMPTY;

public:
    Component() = default;
    Component(Transform local_transform) : m_local_transform(local_transform), GameObject() {}

    void LayoutUpdate(Transform parent_transform) {
        Vector3 rotated = Vector3RotateByQuaternion(m_local_transform.translation, parent_transform.rotation);
        SetPosition(parent_transform.translation + rotated);
        SetRotation(QuaternionMultiply(parent_transform.rotation, m_local_transform.rotation));
        SetScale(m_local_transform.scale * parent_transform.scale);

        LIL_LOG_TRACE("Component calling OnLayoutUpdate");
        LIL_LOG_TRACE(GetTypeInfo().Name());
        OnLayoutUpdate();
        LIL_LOG_TRACE("Component calling OnLayoutUpdate DONE");
    }

    Transform& Local() { return m_local_transform; }

    virtual void SimulationUpdate(Actor& actor, float delta_time) {};
    virtual void Draw() {};
    virtual void DebugUpdate() {};
    virtual void DebugDraw() {};

    void MarkRequired() { m_is_required = true; }
    bool IsRequired() { return m_is_required; }
};
LIL_REFLECT(Component, bases<GameObject>,
    field(m_local_transform)
)
LIL_SER_BEGIN(Component)
LIL_SER_BASE(GameObject)
LIL_SER_FIELD(m_local_transform)
LIL_SER_FIELD(m_is_required)
LIL_SER_END()
