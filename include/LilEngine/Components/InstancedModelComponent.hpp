#pragma once

#include "Component.hpp"
#include "ReflAttributes.hpp"

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

    void ResizeInternal(int size);

public:
    std::string m_model_key = "None";

public:
    LIL_REFLECTABLE()

    InstancedModelComponent() = default;
    virtual ~InstancedModelComponent() = default;

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
        Update();
    }
};
LIL_REFLECT(InstancedModelComponent, bases<Component>,
    field(m_model_key, ModelKeyAttribute())
)
LIL_SER_REGISTER_POLYMORPHIC(InstancedModelComponent)
