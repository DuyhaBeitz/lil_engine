#include "Physics.hpp"
#include <thread>

void PhysicsSystem::Init() {
	JPH::RegisterDefaultAllocator();

	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	JPH::Factory::sInstance = new JPH::Factory();

	JPH::RegisterTypes();

    m_temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

	m_job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

	const JPH::uint cMaxBodies = 1024;
	const JPH::uint cNumBodyMutexes = 0;
	const JPH::uint cMaxBodyPairs = 1024;
	const JPH::uint cMaxContactConstraints = 1024;
	m_broad_phase_layer_interface = std::make_unique<BPLayerInterfaceImpl>();
    m_object_vs_broadphase_layer_filter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_object_vs_object_layer_filter = std::make_unique<ObjectLayerPairFilterImpl>();
    m_physics_system = std::make_unique<JPH::PhysicsSystem>();
	m_physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *m_broad_phase_layer_interface.get(), *m_object_vs_broadphase_layer_filter.get(), *m_object_vs_object_layer_filter.get());

	m_body_activation_listener = std::make_unique<MyBodyActivationListener>();
	m_physics_system->SetBodyActivationListener(m_body_activation_listener.get());
	

	m_contact_listener = std::make_unique<MyContactListener>();
	m_physics_system->SetContactListener(m_contact_listener.get());

	JPH::BodyInterface &body_interface = m_physics_system->GetBodyInterface();

    m_debug_renderer = std::make_unique<RaylibJoltDebugRenderer>();
}

void PhysicsSystem::Step(float delta_time) {
    const int cCollisionSteps = 1;
    m_physics_system->Update(delta_time, cCollisionSteps, m_temp_allocator.get(), m_job_system.get());
}

void PhysicsSystem::DrawDebug() {
    JPH::BodyManager::DrawSettings draw_settings;
    draw_settings.mDrawShape = true;
    draw_settings.mDrawShapeWireframe = true;
    m_physics_system->DrawBodies(draw_settings, m_debug_renderer.get());
}