#pragma once

#include "Component.hpp"

#include "ReflAttributes.hpp"

class InstancedModelComponent : public Component {
private:
    R3D_InstanceBuffer m_instances;
    size_t m_count = 0;

public:
    std::string m_model_key = "None";

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    InstancedModelComponent() = default;
    virtual ~InstancedModelComponent() = default;

    virtual void Draw() override;

    void SetModel(std::string model_key);

    void AddInstance(Transform transform);

    R3D_Model* GetModel() const;
    std::string& ModelKey();
};
LIL_REFLECT(InstancedModelComponent, bases<Component>,
    field(m_model_key, ModelKeyAttribute())
)
LIL_SER_BEGIN(InstancedModelComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_model_key)
LIL_SER_END()
