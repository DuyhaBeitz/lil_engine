#pragma once

#include "Component.hpp"
#include "ResourceManager.hpp"

class ModelComponent : public Component {
public:
    std::string m_model_key;

public:
    LIL_REFLECTABLE()

    ModelComponent(std::string model_key);

    virtual void Draw() override;

    void SetModel(std::string model_key);

    Model* GetModel();
    std::string& ModelKey();
};
REFL_AUTO(type(ModelComponent))