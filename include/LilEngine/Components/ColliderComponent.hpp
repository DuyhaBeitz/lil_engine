#pragma once

#include "Component.hpp"
#include "utils/MathHelper.hpp"

enum class CollisionShapeType : uint8_t {
    SPHERE = 0,
    BOX,
    COUNT
};
LIL_REFLECT(CollisionShapeType, bases<>)

class CollisionShape : public Reflectable {
private:
    rc::Collider* m_collider = nullptr;
    
public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    CollisionShape() = default;

    ~CollisionShape() {
        Destroy();
    }

    CollisionShapeType m_type = ::CollisionShapeType::SPHERE;

    Vector3 m_local_position = Vector3{0.0f, 0.0f, 0.0f};
    Quaternion m_local_rotation = QuaternionIdentity();

    // SPHERE
    float m_radius = 0.0f;

    // BOX
    Vector3 m_half_extends = Vector3{0.0f, 0.0f, 0.0f};

    void Destroy();
    void Create(rc::RigidBody* body);
    
    void Update(rc::RigidBody* body);
};
LIL_REFLECT(CollisionShape, bases<>,
    field(m_type),
    field(m_local_position),
    field(m_local_rotation),
    field(m_radius),
    field(m_half_extends)
)
LIL_SER_BEGIN(CollisionShape)
LIL_SER_FIELD(m_type)
LIL_SER_FIELD(m_local_position)
LIL_SER_FIELD(m_local_rotation)
LIL_SER_FIELD(m_radius)
LIL_SER_FIELD(m_half_extends)
LIL_SER_END()


class ColliderComponent : public Component {
private:
    virtual void Clean() override;
    
    virtual void OnLayoutUpdate() override {
        m_body->setTransform(RcTransform(GetTransform()));

        for (auto& shape : m_shapes) shape.Update(m_body);
    }

protected:
    rc::RigidBody* m_body;
public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    std::vector<CollisionShape> m_shapes = {};

public:

    ColliderComponent(rc::BodyType body_type = rc::BodyType::DYNAMIC);

    rc::RigidBody* GetBody();

    void SetAngularLockAxisFactor(Vector3 lock_axis);

    virtual void SimulationUpdate(Actor& actor) override;
    virtual void DebugDraw() override;
};
LIL_REFLECT_EX(std::vector<CollisionShape>, bases<>, std_vector_CollisionShape)
LIL_REFLECT(ColliderComponent, bases<Component>,
    field(m_shapes)
)
LIL_SER_BEGIN(ColliderComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_shapes)
LIL_SER_END()