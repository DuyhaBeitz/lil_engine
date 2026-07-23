#pragma once

#include "Component.hpp"
#include "ResourceManager.hpp"

#include "ReflAttributes.hpp"

class ModelComponent : public Component {
public:
    std::string m_model_key;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    ModelComponent() = default;
    ModelComponent(std::string model_key);

    virtual void Draw() override;

    void SetModel(std::string model_key);

    Model* GetModel() const;
    std::string& ModelKey();

    RayCollision Raycast(Ray ray) const;
};
LIL_REFLECT(ModelComponent, bases<Component>,
    field(m_model_key, ModelKeyAttribute())
)
LIL_SER_BEGIN(ModelComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_model_key)
LIL_SER_END()
