#include "Components/InstancedModelComponent.hpp"
#include "LilEngine.hpp"

static R3D_InstanceLayout instance_layout = {
    .formats = {
        R3D_INSTANCE_FORMAT_FLOAT32,    // position
        R3D_INSTANCE_FORMAT_SNORM16,    // rotation quaternion
        R3D_INSTANCE_FORMAT_FLOAT16,    // scale
        R3D_INSTANCE_FORMAT_UNORM8,     // color
    },
    .flags = R3D_INSTANCE_POSITION |
                R3D_INSTANCE_ROTATION |
                R3D_INSTANCE_SCALE |
                R3D_INSTANCE_COLOR,
};

typedef struct PackedRotation {
    int16_t x, y, z, w;
} PackedRotation;

typedef struct PackedScale {
    uint16_t x, y, z;
} PackedScale;

void InstancedModelComponent::Draw() {
    Component::Draw();

    if (R3D_Model* m = GetModel()) R3D_DrawModelInstanced(*m, m_instances, m_count);
}

void InstancedModelComponent::SetModel(std::string model_key) {m_model_key = model_key;}
R3D_Model *InstancedModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &InstancedModelComponent::ModelKey() { return m_model_key; }

void InstancedModelComponent::AddInstance(Transform transform) {
    m_count++;

    if (m_count == 1) m_instances = R3D_LoadInstanceBufferEx(m_count, instance_layout);
    else R3D_ResizeInstanceBuffer(&m_instances, m_count, true);

    Vector3* positions        = static_cast<Vector3*       >(R3D_MapInstances(m_instances, R3D_INSTANCE_POSITION, false));
    PackedRotation* rotations = static_cast<PackedRotation*>(R3D_MapInstances(m_instances, R3D_INSTANCE_ROTATION, false));
    PackedScale* scales       = static_cast<PackedScale*   >(R3D_MapInstances(m_instances, R3D_INSTANCE_SCALE, false));
    Color* colors             = static_cast<Color*         >(R3D_MapInstances(m_instances, R3D_INSTANCE_COLOR, false));

    int i = m_count-1;
    positions[i] = transform.translation;
    rotations[i] = (PackedRotation) {
        R3D_PackSnorm16(transform.rotation.x),
        R3D_PackSnorm16(transform.rotation.y),
        R3D_PackSnorm16(transform.rotation.z),
        R3D_PackSnorm16(transform.rotation.w)
    };
    
    scales[i] = (PackedScale) {
        R3D_PackFloat16(transform.scale.x),
        R3D_PackFloat16(transform.scale.y),
        R3D_PackFloat16(transform.scale.z)
    };
    colors[i] = WHITE;

    R3D_UnmapInstances(
        m_instances,
        R3D_INSTANCE_POSITION |
        R3D_INSTANCE_ROTATION |
        R3D_INSTANCE_SCALE |
        R3D_INSTANCE_COLOR
    );
}
