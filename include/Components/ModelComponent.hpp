#pragma once

#include "Component.hpp"
#include "ResourceManager.hpp"

class ModelComponent : public Component {
protected:
    std::string m_model_key;

public:
    ModelComponent(std::string model_key);

    virtual void Draw();

    void SetModel(std::string model_key);
    virtual void Update(Actor& actor) override;

    Model* GetModel();
    std::string& ModelKey();
};