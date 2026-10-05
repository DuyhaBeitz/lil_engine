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


bool InstancedModelComponent::CheckMapping() {
    if (m_positions && m_rotations && m_scales && m_colors) return true;
    LIL_LOG_ERROR("InstancedModelComponent::CheckMapping FAILED");
    return false;
}

void InstancedModelComponent::Draw() {
    Component::Draw();
    if (m_count == 0) return;
    if (R3D_Model* m = GetModel()) R3D_DrawModelInstanced(*m, m_instances, m_count);
}

void InstancedModelComponent::SetModel(std::string model_key) {m_model_key = model_key;}
R3D_Model *InstancedModelComponent::GetModel() const { return Lil::Resources().GetModel(m_model_key); }
std::string &InstancedModelComponent::ModelKey() { return m_model_key; }

void InstancedModelComponent::MapInstances() {
    m_positions = static_cast<Vector3*       >(R3D_MapInstances(m_instances, R3D_INSTANCE_POSITION, false));
    m_rotations = static_cast<PackedRotation*>(R3D_MapInstances(m_instances, R3D_INSTANCE_ROTATION, false));
    m_scales    = static_cast<PackedScale*   >(R3D_MapInstances(m_instances, R3D_INSTANCE_SCALE, false));
    m_colors    = static_cast<Color*         >(R3D_MapInstances(m_instances, R3D_INSTANCE_COLOR, false));
}

void InstancedModelComponent::UnmapInstances() {
    R3D_UnmapInstances(
        m_instances,
        R3D_INSTANCE_POSITION |
        R3D_INSTANCE_ROTATION |
        R3D_INSTANCE_SCALE |
        R3D_INSTANCE_COLOR
    );

    m_positions = nullptr;
    m_rotations = nullptr;
    m_scales    = nullptr;
    m_colors    = nullptr;
}

void InstancedModelComponent::SetInstancePosition(Vector3 position, int i) {if (CheckMapping()) m_positions[i] = position;}
void InstancedModelComponent::SetInstanceRotation(Quaternion rotation, int i) {
    if (CheckMapping()) {
        m_rotations[i] = (PackedRotation) {
            R3D_PackSnorm16(rotation.x),
            R3D_PackSnorm16(rotation.y),
            R3D_PackSnorm16(rotation.z),
            R3D_PackSnorm16(rotation.w)
        };
    }
}
void InstancedModelComponent::SetInstanceScale(Vector3 scale, int i) {
    if (CheckMapping()) {
        m_scales[i] = (PackedScale) {
            R3D_PackFloat16(scale.x),
            R3D_PackFloat16(scale.y),
            R3D_PackFloat16(scale.z)
        };
    }
}
void InstancedModelComponent::SetInstanceTransform(Transform transform, int i) {
    SetInstancePosition(transform.translation, i);
    SetInstanceRotation(transform.rotation, i);
    SetInstanceScale(transform.scale, i);
}

void InstancedModelComponent::SetInstanceColor(Color color, int i) {if (CheckMapping()) {m_colors[i] = color;}}

void InstancedModelComponent::ClearInstances() {
    UnmapInstances();
    R3D_UnloadInstanceBuffer(m_instances);
    m_count = 0;
}

void InstancedModelComponent::Resize(int size) {
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

void InstancedModelComponent::AddInstance(Transform transform) {
    m_count++;

    if (m_count == 1) m_instances = R3D_LoadInstanceBufferEx(m_count, instance_layout);
    else R3D_ResizeInstanceBuffer(&m_instances, m_count, true);

    MapInstances();
    SetInstanceTransform(transform, m_count-1);
    SetInstanceColor(WHITE, m_count-1);
    UnmapInstances();
}
