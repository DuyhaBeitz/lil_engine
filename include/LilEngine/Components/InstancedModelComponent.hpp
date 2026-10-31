#pragma once
#include "Component.hpp"
#include "ReflAttributes.hpp"
#include "Physics.hpp"
#include "Assets.hpp"
#include "Render/InstanceBuffer.hpp"

class InstancedModelComponent : public Component {
private:
    Lil::Render::InstanceBuffer m_buffer;
    std::vector<JPH::BodyID> m_body_ids;

    void ClearCollisions();
    JPH::BodyID CreateBody(const InstanceInfo& instance);

public:
    Lil::Assets::Model m_model_key;
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

    void Update();                          // upload + rebuild collision bodies

    void SetModel(std::string model_key);
    Lil::Assets::Model& ModelKey();

    void SetInstancePosition(Vector3 position, int i);
    void SetInstanceRotation(Quaternion rotation, int i);
    void SetInstanceScale(Vector3 scale, int i);
    void SetInstanceTransform(Transform transform, int i);
    void SetInstanceColor(Color color, int i);

    void Resize(int size);
    void ClearInstances();

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
    field(m_model_key),
    field(m_collides),
    field(m_collision_rel_offset),
    field(m_collision_rel_half_extent)
)
LIL_SER_REGISTER_POLYMORPHIC(InstancedModelComponent)