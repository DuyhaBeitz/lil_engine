#pragma once

#include "Component.hpp"
#include "utils/MathHelper.hpp"

class ColliderComponent : public Component {
private:
    virtual void Clean() override;
    
    virtual void OnLayoutUpdate() override {
        m_body->setTransform(RcTransform(GetTransform()));
    }

protected:
    rc::RigidBody* m_body;
public:
    LIL_REFLECTABLE()

public:

    ColliderComponent(rc::BodyType body_type = rc::BodyType::DYNAMIC);

    rc::RigidBody* GetBody();

    void SetAngularLockAxisFactor(Vector3 lock_axis);

    virtual void SimulationUpdate(Actor& actor) override;
    virtual void DebugDraw() override;
};
LIL_REFLECT(ColliderComponent, bases<Component>)