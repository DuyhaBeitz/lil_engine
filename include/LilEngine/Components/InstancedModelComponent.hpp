#pragma once

#include "Component.hpp"

#include "ReflAttributes.hpp"

typedef struct PackedRotation {
    int16_t x, y, z, w;
} PackedRotation;

typedef struct PackedScale {
    uint16_t x, y, z;
} PackedScale;


class InstancedModelComponent : public Component {
private:
    R3D_InstanceBuffer m_instances;
    size_t m_count = 0;

    Vector3* m_positions;
    PackedRotation* m_rotations;
    PackedScale* m_scales;
    Color* m_colors;

    bool CheckMapping();

public:
    std::string m_model_key = "None";

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    InstancedModelComponent() = default;
    virtual ~InstancedModelComponent() = default;

    virtual void Draw() override;

    void SetModel(std::string model_key);

    void MapInstances();
    void UnmapInstances();

    // In between MapInstances() and UnmapInstances();
    void SetInstancePosition(Vector3 position, int i);
    void SetInstanceRotation(Quaternion rotation, int i);
    void SetInstanceScale(Vector3 scale, int i);
    void SetInstanceTransform(Transform transform, int i);
    void SetInstanceColor(Color color, int i);

    void ClearInstances();
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
