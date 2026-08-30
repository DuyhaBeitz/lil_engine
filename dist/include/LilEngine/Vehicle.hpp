#pragma once

#include "Actor.hpp"
#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "ReflAttributes.hpp"
#include <array>

struct VehicleConfig {
    float           wheel_radius = 0.3f;
	float           wheel_width = 0.1f;
	float           half_vehicle_length = 2.0f;
	float           half_vehicle_width = 0.9f;
	float           half_vehicle_height = 0.2f;

    float			InitialRollAngle = 0;
	float			MaxRollAngle = JPH::DegreesToRadians(60.0f);
	float			MaxSteeringAngle = JPH::DegreesToRadians(30.0f);
	bool			FourWheelDrive = false;
	bool			AntiRollbar = true;
	bool			LimitedSlipDifferentials = true;
	bool			OverrideGravity = false;					///< If true, gravity is overridden to always oppose the ground normal
	float			MaxEngineTorque = 500.0f;
	float			ClutchStrength = 10.0f;
	float			FrontCasterAngle = 0.0f;
	float			FrontKingPinAngle = 0.0f;
	float			FrontCamber = 0.0f;
	float			FrontToe = 0.0f;
	float			FrontSuspensionForwardAngle = 0.0f;
	float			FrontSuspensionSidewaysAngle = 0.0f;
	float			FrontSuspensionMinLength = 0.3f;
	float			FrontSuspensionMaxLength = 0.5f;
	float			FrontSuspensionFrequency = 1.5f;
	float			FrontSuspensionDamping = 0.5f;
	float			RearSuspensionForwardAngle = 0.0f;
	float			RearSuspensionSidewaysAngle = 0.0f;
	float			RearCasterAngle = 0.0f;
	float			RearKingPinAngle = 0.0f;
	float			RearCamber = 0.0f;
	float			RearToe = 0.0f;
	float			RearSuspensionMinLength = 0.3f;
	float			RearSuspensionMaxLength = 0.5f;
	float			RearSuspensionFrequency = 1.5f;
	float			RearSuspensionDamping = 0.5f;
};

class Vehicle : public Actor {
protected:
    JPH::BodyID m_body_id;
    JPH::Ref<JPH::VehicleConstraint> mVehicleConstraint;
    JPH::Ref<JPH::VehicleCollisionTester> mTester;

	virtual void DebugDraw() override;

public:
    Vehicle(const VehicleConfig& config = VehicleConfig{});
    virtual ~Vehicle();

    void LayoutUpdate() override;
    void SimulationUpdate(float delta_time) override;

	std::array<Transform, 4> GetWheelTransforms();

    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

protected:

	VehicleConfig m_vehicle_config;

	// Player input
	float						mForward = 0.0f;
	float						mPreviousForward = 1.0f;					///< Keeps track of last car direction so we know when to brake and when to accelerate
	float						mRight = 0.0f;
	float						mBrake = 0.0f;
	float						mHandBrake = 0.0f;
};
LIL_REFLECT(Vehicle, bases<Actor>)
LIL_SER_BEGIN(Vehicle)
LIL_SER_BASE(Actor)
LIL_SER_END()
