#pragma once

#include "Component.hpp"
#include "ReflAttributes.hpp"
#include "Physics.hpp"

struct InstanceInfo : public Reflectable {
    InstanceInfo() = default;
    InstanceInfo(Vector3 pos, Quaternion rot, Vector3 scal, Color col);
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Vector3    position;
    Quaternion rotation;
    Vector3    scale;
    Color      color;
};
LIL_REFLECT(InstanceInfo, bases<>,
    field(position),
    field(rotation),
    field(scale),
    field(color)
)

LIL_SER_BEGIN(InstanceInfo)
LIL_SER_FIELD(position)
LIL_SER_FIELD(rotation)
LIL_SER_FIELD(scale)
LIL_SER_FIELD(color)
LIL_SER_END()

class InstancedModelComponent : public Component {
private:
    R3D_InstanceBuffer m_instances;
    int m_count = 0;
    std::vector<JPH::BodyID> m_body_ids;

    void ResizeInternal(int size);
    void ClearCollisions();
    JPH::BodyID CreateBody(const InstanceInfo& instance);

public:
    std::string m_model_key = "None";
    bool m_collides = false;
    bool m_update_collisions = false;
    Vector3 m_collision_rel_offset = Vector3{0.0f, 0.0f, 0.0f};
    Vector3 m_collision_rel_half_extent  = Vector3{1.0f, 1.0f, 1.0f};

public:
    LIL_REFLECTABLE()

    InstancedModelComponent() = default;
    virtual ~InstancedModelComponent() = default;

    virtual void DebugDraw() override;
    virtual void Draw() override;

    void Update();

    void SetModel(std::string model_key);


    // In between MapInstances() and UnmapInstances();
    void SetInstancePosition(Vector3 position, int i);
    void SetInstanceRotation(Quaternion rotation, int i);
    void SetInstanceScale(Vector3 scale, int i);
    void SetInstanceTransform(Transform transform, int i);
    void SetInstanceColor(Color color, int i);

    void Resize(int size);
    void ClearInstances();

    R3D_Model* GetModel() const;
    std::string& ModelKey();
    std::vector<InstanceInfo> m_infos;

    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Component>(this));
        ar(m_model_key, m_infos);
        ar(m_collides, m_collision_rel_offset, m_collision_rel_half_extent);
        Update();
    }
};
LIL_REFLECT(InstancedModelComponent, bases<Component>,
    field(m_model_key, ModelKeyAttribute()),
    field(m_collides),
    field(m_collision_rel_offset),
    field(m_collision_rel_half_extent)
)
LIL_SER_REGISTER_POLYMORPHIC(InstancedModelComponent)
