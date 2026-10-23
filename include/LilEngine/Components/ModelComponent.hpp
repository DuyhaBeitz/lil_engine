#pragma once

#include "Component.hpp"
#include "Assets.hpp"

#include "Serialization.hpp"
#include "Reflection.hpp"

class ModelComponent : public Component {
public:
    Lil::Assets::Model m_model_key;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    ModelComponent() = default;
    ModelComponent(Lil::Assets::Model model_key);

    virtual void Draw() override;

    void SetModel(std::string model_key);

    //R3D_Model* GetModel() const;
    Lil::Assets::Model ModelKey();

    virtual RayCollision Raycast(Ray ray) const override;
};
LIL_REFLECT(ModelComponent, bases<Component>,
    field(m_model_key)
)
LIL_SER_BEGIN(ModelComponent)
LIL_SER_BASE(Component)
LIL_SER_FIELD(m_model_key)
LIL_SER_END()
