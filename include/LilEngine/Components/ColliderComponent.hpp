#pragma once

#include "Component.hpp"
#include "utils/MathHelper.hpp"
#include "ReflAttributes.hpp"
#include <list>

enum class CollisionShapeType : uint8_t {
    SPHERE = 0,
    BOX,
    HEIGHTMAP,
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

    virtual ~CollisionShape() {
        Destroy();
    }

    CollisionShapeType m_type = ::CollisionShapeType::SPHERE;

    Vector3 m_local_position = Vector3{0.0f, 0.0f, 0.0f};
    Quaternion m_local_rotation = QuaternionIdentity();

    // SPHERE
    float m_radius = 0.0f;

    // BOX
    Vector3 m_half_extends = Vector3{0.0f, 0.0f, 0.0f};

    // Heightmap
    std::string m_heightmap_texture_key = "";
    Vector3 m_map_size = Vector3{1.0f, 1.0f, 1.0f};

    void Destroy();
    void Create(rc::RigidBody* body);
    
    void Update(rc::RigidBody* body);
};
LIL_REFLECT(CollisionShape, bases<>,
    field(m_type),
    field(m_local_position),
    field(m_local_rotation),
    field(m_radius),
    field(m_half_extends),
    field(m_heightmap_texture_key, TextureKeyAttribute()),
    field(m_map_size)
)
LIL_SER_BEGIN(CollisionShape)
LIL_SER_FIELD(m_type)
LIL_SER_FIELD(m_local_position)
LIL_SER_FIELD(m_local_rotation)
LIL_SER_FIELD(m_radius)
LIL_SER_FIELD(m_half_extends)
LIL_SER_FIELD(m_heightmap_texture_key)
LIL_SER_FIELD(m_map_size)
LIL_SER_END()

enum class BodyType : int8_t {
    STATIC = 0,
    KINEMATIC,
    DYNAMIC,
    COUNT
};
LIL_REFLECT(BodyType, bases<>)


inline rc::BodyType GetBodyType(const BodyType body_type) {
    switch (body_type) {
        case BodyType::STATIC:
            return rc::BodyType::STATIC;
            break;
        case BodyType::DYNAMIC:
            return rc::BodyType::DYNAMIC;
            break;
        case BodyType::KINEMATIC:
            return rc::BodyType::KINEMATIC;
            break;
    }
    return rc::BodyType::DYNAMIC;
}

class ColliderComponent : public Component {
private:
    
    virtual void OnLayoutUpdate() override {
        if (m_body) {
            LIL_LOG_TRACE("Setting body params");
            m_body->setTransform(RcTransform(GetTransform()));
            LIL_LOG_TRACE("1");
            m_body->setType(GetBodyType(m_type));
            LIL_LOG_TRACE("2");
            m_body->setLinearVelocity(RcVector3(m_linear_velocity));
            LIL_LOG_TRACE("2");
            m_body->setAngularVelocity(RcVector3(m_angular_velocity));
            LIL_LOG_TRACE("Setting body params DONE");

            LIL_LOG_TRACE("ColliderComponent updating shapes");
            for (auto& shape : m_shapes) shape.Update(m_body);
            LIL_LOG_TRACE("ColliderComponent updating shapes DONE");
        }
        else {
            LIL_LOG_TRACE("m_body null");
        }
    }

protected:
    rc::RigidBody* m_body;
public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    //std::vector<CollisionShape> m_shapes = {};
    std::list<CollisionShape> m_shapes = {};
    BodyType m_type = BodyType::STATIC;
    Vector3 m_linear_velocity = Vector3{0.0f, 0.0f, 0.0f};
    Vector3 m_angular_velocity = Vector3{0.0f, 0.0f, 0.0f};

public:
    ColliderComponent(BodyType body_type = BodyType::STATIC);
    virtual ~ColliderComponent();

    rc::RigidBody* GetBody();

    void SetAngularLockAxisFactor(Vector3 lock_axis);

    virtual void SimulationUpdate(Actor& actor) override;
    virtual void DebugUpdate() override;
    virtual void DebugDraw() override;

    CollisionShape* AddShape(CollisionShape shape);

    CollisionShape* GetFirstShape(CollisionShapeType type) {
        for (auto& shape : m_shapes) {
            if (shape.m_type == type) return &shape;
        }
        return nullptr;
    }
};
LIL_REFLECT_EX(std::vector<CollisionShape>, bases<>, std_vector_CollisionShape)
LIL_REFLECT_EX(std::list<CollisionShape>, bases<>, std_list_CollisionShape)
LIL_REFLECT(ColliderComponent, bases<Component>,
    field(m_shapes),
    field(m_type),
    field(m_linear_velocity),
    field(m_angular_velocity)
)
LIL_SER_BEGIN(ColliderComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_shapes)
LIL_SER_FIELD(m_type)
LIL_SER_FIELD(m_linear_velocity)
LIL_SER_FIELD(m_angular_velocity)
LIL_SER_END()