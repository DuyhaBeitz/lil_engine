#include "Components/ColliderComponent.hpp"

#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "Physics.hpp"

#include "LilEngine.hpp"
#include <utils/ColliderHelper.hpp>

JPH::RefConst<JPH::Shape> CollisionShape::CreateJoltShape() const {
    switch (m_type) {
        case CollisionShapeType::SPHERE:
            return new JPH::SphereShape(m_radius);
            
        case CollisionShapeType::BOX:
            return new JPH::BoxShape(JphVector3(m_half_extends));
            
        //case CollisionShapeType::HEIGHTMAP: {
            //if (Lil::Resources().TextureExists(m_heightmap_texture_key)) {
                // In Jolt, provide a float array of height samples to HeightFieldShapeSettings
                // Example skeleton of heightmap setup:
                // JPH::HeightFieldShapeSettings settings(samples.data(), offset, scale, sampleCount);
                // return settings.Create().Get();
            //}
            //break;
    }
    return new JPH::SphereShape(0.5f); // Fallback
}

ColliderComponent::ColliderComponent(BodyType body_type) : Component(), m_type(body_type) {
    auto& body_interface = Lil::Physics().GetBodyInterface();
    
    // Create a default empty compound shape
    JPH::StaticCompoundShapeSettings compound_settings;

    compound_settings.AddShape(
        JPH::Vec3::sZero(), 
        JPH::Quat::sIdentity(), 
        new JPH::BoxShapeSettings(JPH::Vec3(0.5f, 0.5f, 0.5f))
    );

    JPH::RefConst<JPH::Shape> default_shape = compound_settings.Create().Get();

    JPH::EMotionType motion_type = GetJoltMotionType(body_type);
    JPH::ObjectLayer layer = (motion_type == JPH::EMotionType::Static) ? JPH::Layers::NON_MOVING : JPH::Layers::MOVING;

    JPH::BodyCreationSettings settings(
        default_shape,
        JphVector3(GetTransform().translation),
        JphQuat(GetTransform().rotation),
        motion_type,
        layer
    );
    settings.mAllowDynamicOrKinematic = true;

    const JPH::Body* body = body_interface.CreateBody(settings);
    if (!body) {
        LIL_LOG_ERROR("Failed to create Jolt Body (max bodies limit reached or invalid settings)!");
        return;
    }
    m_body_id = body->GetID();
    body_interface.AddBody(m_body_id, JPH::EActivation::Activate);
}

ColliderComponent::~ColliderComponent() {
    if (!m_body_id.IsInvalid()) {
        auto& body_interface = Lil::Physics().GetBodyInterface();
        body_interface.RemoveBody(m_body_id);
        body_interface.DestroyBody(m_body_id);
    }
}

void ColliderComponent::RebuildShapes() {
    if (m_body_id.IsInvalid() || m_shapes.empty()) return;

    // In Jolt, combine all component shapes into a compound shape
    JPH::StaticCompoundShapeSettings compound_settings;
    for (const auto& shape : m_shapes) {
        compound_settings.AddShape(
            JphVector3(shape.m_local_position),
            JphQuat(shape.m_local_rotation),
            shape.CreateJoltShape()
        );
    }

    auto shape_result = compound_settings.Create();
    if (shape_result.IsValid()) {
        Lil::Physics().GetBodyInterface().SetShape(m_body_id, shape_result.Get(), true, JPH::EActivation::Activate);
    }    
}

void ColliderComponent::OnLayoutUpdate() {
    if (m_body_id.IsInvalid()) return;

    auto& bi = Lil::Physics().GetBodyInterface();
    bi.SetPositionAndRotation(m_body_id, JphVector3(GetTransform().translation), JphQuat(GetTransform().rotation), JPH::EActivation::Activate);
    bi.SetMotionType(m_body_id, GetJoltMotionType(m_type), JPH::EActivation::Activate);
    bi.SetLinearVelocity(m_body_id, JphVector3(m_linear_velocity));
    bi.SetAngularVelocity(m_body_id, JphVector3(m_angular_velocity));

    RebuildShapes();
}

void ColliderComponent::SimulationUpdate(Actor& actor) {
    if (m_body_id.IsInvalid()) return;

    auto& bi = Lil::Physics().GetBodyInterface();  

    Transform t = GetTransform();
    t.translation = RlVector3(bi.GetPosition(m_body_id));
    t.rotation = RlQuat(bi.GetRotation(m_body_id));
    SetTransform(t);

    Transform actorWorld = actor.GetTransform();
    actorWorld.rotation = QuaternionMultiply(m_transform.rotation, QuaternionInvert(m_local_transform.rotation));
    actorWorld.translation = m_transform.translation - m_local_transform.translation;
    actorWorld.scale = GetScale() / Local().scale;
    actor.SetTransform(actorWorld);

    m_linear_velocity = RlVector3(bi.GetLinearVelocity(m_body_id));
    m_angular_velocity = RlVector3(bi.GetAngularVelocity(m_body_id));
}
