#pragma once

#include "Physics.hpp"
#include "Component.hpp"
#include "utils/MathHelper.hpp"
#include "ReflAttributes.hpp"
#include <list>

enum class CollisionShapeType : uint8_t { SPHERE = 0, BOX, HEIGHTMAP, COUNT };
enum class BodyType : int8_t { STATIC = 0, KINEMATIC, DYNAMIC, COUNT };

inline JPH::EMotionType GetJoltMotionType(BodyType type) {
    switch (type) {
        case BodyType::STATIC:    return JPH::EMotionType::Static;
        case BodyType::KINEMATIC: return JPH::EMotionType::Kinematic;
        case BodyType::DYNAMIC:   return JPH::EMotionType::Dynamic;
        default:                  return JPH::EMotionType::Dynamic;
    }
}

class CollisionShape : public Reflectable {
public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    CollisionShapeType m_type = ::CollisionShapeType::SPHERE;
    Vector3 m_local_position = Vector3{0.0f, 0.0f, 0.0f};
    Quaternion m_local_rotation = QuaternionIdentity();

    float m_radius = 1.0f;
    Vector3 m_half_extends = Vector3{0.0f, 0.0f, 0.0f};
    std::string m_heightmap_texture_key = "";
    Vector3 m_map_size = Vector3{1.0f, 1.0f, 1.0f};

    JPH::RefConst<JPH::Shape> CreateJoltShape() const;
    bool m_needs_rebuild = true;
};

class ColliderComponent : public Component {
private:
    JPH::BodyID m_body_id;

    virtual void OnLayoutUpdate() override;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    ColliderComponent(BodyType body_type = BodyType::STATIC);
    virtual ~ColliderComponent();

    std::list<CollisionShape> m_shapes = {};
    BodyType m_type = BodyType::STATIC;
    Vector3 m_linear_velocity = Vector3{0.0f, 0.0f, 0.0f};
    Vector3 m_angular_velocity = Vector3{0.0f, 0.0f, 0.0f};

    JPH::BodyID GetBodyID() const { return m_body_id; }
    void RebuildShapes();
    CollisionShape* AddShape(CollisionShape shape);

    virtual void SimulationUpdate(Actor& actor, float delta_time) override;

    CollisionShape* GetFirstShape(CollisionShapeType type) {
        for (auto& shape : m_shapes) {
            if (shape.m_type == type) return &shape;
        }
        return nullptr;
    }
};


LIL_REFLECT(CollisionShapeType, bases<>)
LIL_REFLECT(BodyType, bases<>)

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