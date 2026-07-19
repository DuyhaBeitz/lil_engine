#pragma once

#include "Component.hpp"
#include "ResourceManager.hpp"

#include "ReflAttributes.hpp"

class ModelComponent : public Component {
public:
    std::string m_model_key;

public:
    LIL_REFLECTABLE()

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