#pragma once

#include "CommonIncludes.hpp"

class Actor {
protected:
    rc::RigidBody* m_body;

public:
    Actor(rc::BodyType body_type = rc::BodyType::DYNAMIC, Vector3 position = Vector3{0.0f, 0.0f, 0.0f});

    virtual void Draw() = 0;

    Vector3 GetAxisAngle(float& angle);
    Vector3 GetPosition();
    
    void SetPosition(Vector3 position);

    rc::RigidBody* GetBody();

    void SetAngularLockAxisFactor(Vector3 lock_axis);
};