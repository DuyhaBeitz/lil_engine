#include "Physics.hpp"

void PhysicsSystem::Init() {
    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
	// This needs to be done before any other Jolt function is called.
	JPH::RegisterDefaultAllocator();

	// Install trace and assert callbacks
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	JPH::Factory::sInstance = new JPH::Factory();

	JPH::RegisterTypes();

    m_temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

	m_job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 1);

	std::cout << "1" << std::endl;
	const uint cMaxBodies = 1024;
	const uint cNumBodyMutexes = 0;
	const uint cMaxBodyPairs = 1024;
	const uint cMaxContactConstraints = 1024;
	std::cout << "2" << std::endl;
	m_broad_phase_layer_interface = std::make_unique<BPLayerInterfaceImpl>();
	std::cout << "3" << std::endl;
    m_object_vs_broadphase_layer_filter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
	std::cout << "4" << std::endl;
    m_object_vs_object_layer_filter = std::make_unique<ObjectLayerPairFilterImpl>();
	std::cout << "5" << std::endl;

    m_physics_system = std::make_unique<JPH::PhysicsSystem>();
	std::cout << "6" << std::endl;
	m_physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *m_broad_phase_layer_interface.get(), *m_object_vs_broadphase_layer_filter.get(), *m_object_vs_object_layer_filter.get());
	std::cout << "7" << std::endl;

	m_body_activation_listener = std::make_unique<MyBodyActivationListener>();
	m_physics_system->SetBodyActivationListener(m_body_activation_listener.get());
	

	m_contact_listener = std::make_unique<MyContactListener>();
	m_physics_system->SetContactListener(m_contact_listener.get());
	std::cout << "4" << std::endl;

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