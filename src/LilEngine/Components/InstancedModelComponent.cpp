#include "Components/InstancedModelComponent.hpp"
#include "LilEngine.hpp"
#include "utils/ColliderHelper.hpp"
#include <utils/MathHelper.hpp>

void InstancedModelComponent::Draw() {
    Component::Draw();
    if (m_infos.empty()) return;
    // lazy upload: covers Resize(), copied components, and a rebuilt GPU context
    if (m_buffer.Count() != int(m_infos.size())) m_buffer.Upload(m_infos);
    m_buffer.Draw(m_model_key);
}

void InstancedModelComponent::SetModel(std::string model_key) { m_model_key = model_key; }
Lil::Assets::Model& InstancedModelComponent::ModelKey() { return m_model_key; }

void InstancedModelComponent::Update() {
    m_buffer.Upload(m_infos);

    if (m_collides) {
        ClearCollisions();
        Lil::Physics().GetSystem()->OptimizeBroadPhase();
        for (const InstanceInfo& info : m_infos) {
            JPH::BodyID id = CreateBody(info);
            if (!id.IsInvalid()) m_body_ids.push_back(id);
        }
    } else if (!m_body_ids.empty()) {
        ClearCollisions();
    }
}

void InstancedModelComponent::Resize(int size)  { m_infos.resize(size < 0 ? 0 : size); }
void InstancedModelComponent::ClearInstances()  { m_infos.clear(); m_buffer.Clear(); }

void InstancedModelComponent::DebugDraw() {
    Component::DebugDraw();

    if (m_collides) {
        for (auto id : m_body_ids) {
            DrawDebugPhysicsBody(id);
        }
    }
}

void InstancedModelComponent::ClearCollisions() {
    auto& body_interface = Lil::Physics().GetBodyInterface();
    for (JPH::BodyID id : m_body_ids) {
        if (id.IsInvalid()) continue;
        if (body_interface.IsAdded(id)) body_interface.RemoveBody(id);
        body_interface.DestroyBody(id);
    }
    m_body_ids.clear();
}

JPH::BodyID InstancedModelComponent::CreateBody(const InstanceInfo &instance) {
    const Vector3 half_extent = instance.scale * m_collision_rel_half_extent;

    JPH::RefConst<JPH::Shape> shape = new JPH::BoxShape(JphVector3NonNullScale(half_extent));

    JPH::BodyCreationSettings settings(
        shape,
        JphVector3(
            instance.position + Vector3RotateByQuaternion(half_extent*m_collision_rel_offset, instance.rotation)
        ),
        JphQuat(instance.rotation),
        JPH::EMotionType::Static,
        JPH::Layers::NON_MOVING
    );

    auto& body_interface = Lil::Physics().GetBodyInterface();
    auto* ps = Lil::Physics().GetSystem();
    return body_interface.CreateAndAddBody(settings, JPH::EActivation::DontActivate);
}