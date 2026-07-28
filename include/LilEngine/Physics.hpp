#pragma once

#include "JoltWrapper.hpp"
#include "PhysicsDebugRenderer.hpp"

class PhysicsSystem {
private:
    std::unique_ptr<JPH::TempAllocatorImpl> m_temp_allocator;
    std::unique_ptr<JPH::JobSystemThreadPool> m_job_system;
    std::unique_ptr<JPH::PhysicsSystem> m_physics_system;

    // Jolt required interfaces (implement basic versions of these)
    std::unique_ptr<BPLayerInterfaceImpl> m_broad_phase_layer_interface;
    std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilter> m_object_vs_broadphase_layer_filter;
    std::unique_ptr<JPH::ObjectLayerPairFilter> m_object_vs_object_layer_filter;

    std::unique_ptr<MyBodyActivationListener> m_body_activation_listener;
    std::unique_ptr<MyContactListener> m_contact_listener;
    std::unique_ptr<RaylibJoltDebugRenderer> m_debug_renderer;


public:
    PhysicsSystem() = default;;
    ~PhysicsSystem() = default;;

    void Init();
    void Step(float delta_time);
    void DrawDebug();

    JPH::PhysicsSystem* GetSystem() { return m_physics_system.get(); }
    JPH::BodyInterface& GetBodyInterface() { return m_physics_system->GetBodyInterface(); }
};