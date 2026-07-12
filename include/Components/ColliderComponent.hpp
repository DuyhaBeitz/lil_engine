#pragma once

#include "Component.hpp"
#include "utils/MathHelper.hpp"

class ColliderComponent : public Component {
private:
    void UpdateBodyTransform() {
        //m_body->setTransform(RcTransform(GetTransform()));
    }

protected:
    rc::RigidBody* m_body;

public:
    ColliderComponent(rc::BodyType body_type = rc::BodyType::DYNAMIC);

    // virtual void SetTransform(Transform transform) override {m_transform = transform; UpdateBodyTransform(); }
    // virtual void SetPosition(Vector3 position) override {m_transform.translation = position; UpdateBodyTransform(); }
    // virtual void SetRotation(Quaternion rotation) override {m_transform.rotation = rotation; UpdateBodyTransform(); }
    // virtual void SetScale(Vector3 scale) override {m_transform.scale = scale; UpdateBodyTransform(); }

    rc::RigidBody* GetBody();

    void SetAngularLockAxisFactor(Vector3 lock_axis);

    virtual void Update(Actor& actor) override;
    virtual void DebugDraw() override;
};