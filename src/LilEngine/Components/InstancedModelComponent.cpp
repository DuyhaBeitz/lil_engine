#include "Components/InstancedModelComponent.hpp"
#include "LilEngine.hpp"

typedef struct PackedRotation {
    int16_t x, y, z, w;
} PackedRotation;

typedef struct PackedScale {
    uint16_t x, y, z;
} PackedScale;

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

InstanceInfo::InstanceInfo(Vector3 pos, Quaternion rot, Vector3 scal, Color col) {
    position = pos;
    rotation = rot;
    scale    = scal;
    color    = col;
}

void InstancedModelComponent::Draw() {
    Component::Draw();
    if (m_count == 0) return;
    if (R3D_Model* m = GetModel()) R3D_DrawModelInstanced(*m, m_instances, m_count);
}

void InstancedModelComponent::SetModel(std::string model_key) {m_model_key = model_key;}
R3D_Model *InstancedModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &InstancedModelComponent::ModelKey() { return m_model_key; }

void InstancedModelComponent::Update() {
    ResizeInternal(m_infos.size());
    if (m_count == 0) return;

    Vector3*        positions = static_cast<Vector3*       >(R3D_MapInstances(m_instances, R3D_INSTANCE_POSITION, false));
    PackedRotation* rotations = static_cast<PackedRotation*>(R3D_MapInstances(m_instances, R3D_INSTANCE_ROTATION, false));
    PackedScale*    scales    = static_cast<PackedScale*   >(R3D_MapInstances(m_instances, R3D_INSTANCE_SCALE, false));
    Color*          colors    = static_cast<Color*         >(R3D_MapInstances(m_instances, R3D_INSTANCE_COLOR, false));

    for (int i = 0; i < m_infos.size(); i++) {
        positions[i] = m_infos[i].position;
        rotations[i] = (PackedRotation) {
            R3D_PackSnorm16(m_infos[i].rotation.x),
            R3D_PackSnorm16(m_infos[i].rotation.y),
            R3D_PackSnorm16(m_infos[i].rotation.z),
            R3D_PackSnorm16(m_infos[i].rotation.w)
        };

        scales[i] = (PackedScale) {
            R3D_PackFloat16(m_infos[i].scale.x),
            R3D_PackFloat16(m_infos[i].scale.y),
            R3D_PackFloat16(m_infos[i].scale.z)
        };

        colors[i] = m_infos[i].color;
    }


    R3D_UnmapInstances(
        m_instances,
        R3D_INSTANCE_POSITION |
        R3D_INSTANCE_ROTATION |
        R3D_INSTANCE_SCALE |
        R3D_INSTANCE_COLOR
    );
}

void InstancedModelComponent::SetInstancePosition(Vector3 position, int i) {m_infos[i].position = position;}
void InstancedModelComponent::SetInstanceRotation(Quaternion rotation, int i) {m_infos[i].rotation = rotation;}
void InstancedModelComponent::SetInstanceScale(Vector3 scale, int i) {m_infos[i].scale = scale;}
void InstancedModelComponent::SetInstanceColor(Color color, int i) {m_infos[i].color = color;}
void InstancedModelComponent::SetInstanceTransform(Transform transform, int i) {
    SetInstancePosition(transform.translation, i);
    SetInstanceRotation(transform.rotation, i);
    SetInstanceScale(transform.scale, i);
}

void InstancedModelComponent::ClearInstances() {
    R3D_UnloadInstanceBuffer(m_instances);
    m_count = 0;
    m_infos.clear();
}

void InstancedModelComponent::ResizeInternal(int size) {
    if (size < 0) size = 0;
    if (size == m_count) return;
    if (size == 0) {
        ClearInstances();
        return;
    }

    if (m_count == 0) {
        m_instances = R3D_LoadInstanceBufferEx(size, instance_layout);
    }
    else if (size > m_count) {
        R3D_ResizeInstanceBuffer(&m_instances, size, true);
    }
    // if size < m_count don't do anything, R3D_DrawModelInstanced accepts count

    m_count = size;
}

void InstancedModelComponent::Resize(int size) {
    ResizeInternal(size);
    m_infos.resize(size);
}
