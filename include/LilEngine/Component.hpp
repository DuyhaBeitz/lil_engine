#pragma once

#include "Reflection.hpp"
#include "GameObject.hpp"
#include "utils/MathHelper.hpp"

class Actor;

class Component : public GameObject {
    // required components cannot be detached, they are native for actor that uses them
    // e.g. specifically setup model component for heightmap actor
    bool m_is_required = false;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    Transform m_local_transform = TRANSFORM_EMPTY;

public:
    Component() = default;
    Component(Transform local_transform);

    Transform& Local() { return m_local_transform; }

    void ApplyParentTransform(Transform parent_transform);
    
    virtual void LayoutUpdate();
    virtual void SimulationUpdate(Actor& actor, float delta_time);
    virtual void Draw();
    virtual void DebugUpdate();
    virtual void DebugDraw();

    virtual RayCollision Raycast(Ray ray) const;

    void MarkRequired();
    bool IsRequired();
};
LIL_REFLECT(Component, bases<GameObject>,
    field(m_local_transform)
)
LIL_SER_BEGIN(Component)
LIL_SER_BASE(GameObject)
LIL_SER_FIELD(m_local_transform)
LIL_SER_FIELD(m_is_required)
LIL_SER_END()
