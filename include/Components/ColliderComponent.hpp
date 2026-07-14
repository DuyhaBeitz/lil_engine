#pragma once

#include "Component.hpp"
#include "utils/MathHelper.hpp"

class ColliderComponent : public Component {
public:
    virtual void OnLayoutUpdate() {
        m_body->setTransform(RcTransform(GetTransform()));
    }

protected:
    rc::RigidBody* m_body;

public:
    LIL_REFLECTABLE()

    ColliderComponent(rc::BodyType body_type = rc::BodyType::DYNAMIC);

    rc::RigidBody* GetBody();

    void SetAngularLockAxisFactor(Vector3 lock_axis);

    virtual void SimulationUpdate(Actor& actor) override;
    virtual void DebugDraw() override;
};
REFL_AUTO(type(ColliderComponent, bases<Component>))