#pragma once

#include "Actor.hpp"
#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "ReflAttributes.hpp"
#include <array>

struct VehicleConfig : public Reflectable {
	LIL_SERIALIZABLE()
	LIL_REFLECTABLE()

    float           wheel_radius = 0.3f;
	float           wheel_width = 0.1f;
	float           half_vehicle_length = 2.0f;
	float           half_vehicle_width = 0.9f;
	float           half_vehicle_height = 0.2f;

	float           mass = 1500.0f;

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
LIL_REFLECT(VehicleConfig, bases<>,
	field(wheel_radius),
	field(wheel_width),
	field(half_vehicle_length),
	field(half_vehicle_width),
	field(half_vehicle_height),

	field(mass),

	field(InitialRollAngle),
	field(MaxRollAngle),
	field(MaxSteeringAngle),
	field(FourWheelDrive),
	field(AntiRollbar),
	field(LimitedSlipDifferentials),
	field(OverrideGravity),
	field(MaxEngineTorque),
	field(ClutchStrength),
	field(FrontCasterAngle),
	field(FrontKingPinAngle),
	field(FrontCamber),
	field(FrontToe),
	field(FrontSuspensionForwardAngle),
	field(FrontSuspensionSidewaysAngle),
	field(FrontSuspensionMinLength),
	field(FrontSuspensionMaxLength),
	field(FrontSuspensionFrequency),
	field(FrontSuspensionDamping),
	field(RearSuspensionForwardAngle),
	field(RearSuspensionSidewaysAngle),
	field(RearCasterAngle),
	field(RearKingPinAngle),
	field(RearCamber),
	field(RearToe),
	field(RearSuspensionMinLength),
	field(RearSuspensionMaxLength),
	field(RearSuspensionFrequency),
	field(RearSuspensionDamping)
)
LIL_SER_BEGIN(VehicleConfig)
LIL_SER_FIELD(wheel_radius)
LIL_SER_FIELD(wheel_width)
LIL_SER_FIELD(half_vehicle_length)
LIL_SER_FIELD(half_vehicle_width)
LIL_SER_FIELD(half_vehicle_height)

LIL_SER_FIELD(mass)

LIL_SER_FIELD(InitialRollAngle)
LIL_SER_FIELD(MaxRollAngle)
LIL_SER_FIELD(MaxSteeringAngle)
LIL_SER_FIELD(FourWheelDrive)
LIL_SER_FIELD(AntiRollbar)
LIL_SER_FIELD(LimitedSlipDifferentials)
LIL_SER_FIELD(OverrideGravity)
LIL_SER_FIELD(MaxEngineTorque)
LIL_SER_FIELD(ClutchStrength)
LIL_SER_FIELD(FrontCasterAngle)
LIL_SER_FIELD(FrontKingPinAngle)
LIL_SER_FIELD(FrontCamber)
LIL_SER_FIELD(FrontToe)
LIL_SER_FIELD(FrontSuspensionForwardAngle)
LIL_SER_FIELD(FrontSuspensionSidewaysAngle)
LIL_SER_FIELD(FrontSuspensionMinLength)
LIL_SER_FIELD(FrontSuspensionMaxLength)
LIL_SER_FIELD(FrontSuspensionFrequency)
LIL_SER_FIELD(FrontSuspensionDamping)
LIL_SER_FIELD(RearSuspensionForwardAngle)
LIL_SER_FIELD(RearSuspensionSidewaysAngle)
LIL_SER_FIELD(RearCasterAngle)
LIL_SER_FIELD(RearKingPinAngle)
LIL_SER_FIELD(RearCamber)
LIL_SER_FIELD(RearToe)
LIL_SER_FIELD(RearSuspensionMinLength)
LIL_SER_FIELD(RearSuspensionMaxLength)
LIL_SER_FIELD(RearSuspensionFrequency)
LIL_SER_FIELD(RearSuspensionDamping)
LIL_SER_END()

class Vehicle : public Actor {
protected:
    JPH::BodyID m_body_id;
    JPH::Ref<JPH::VehicleConstraint> mVehicleConstraint;
    JPH::Ref<JPH::VehicleCollisionTester> mTester;

	void ConstructVehicle(const VehicleConfig& config = VehicleConfig{});
	void DestroyVehicle();

public:
	Vehicle() = default;
    virtual ~Vehicle();

    virtual void SetupComponents() override;
    void LayoutUpdate() override;
    void SimulationUpdate(float delta_time) override;
	virtual void DebugDraw() override;

	std::array<Transform, 4> GetWheelTransforms();

    LIL_REFLECTABLE()

public:

	VehicleConfig m_vehicle_config;

	// Player input
	float						mForward = 0.0f;
	float						mPreviousForward = 1.0f; ///< Keeps track of last car direction so we know when to brake and when to accelerate
	float						mRight = 0.0f;
	float						mBrake = 0.0f;
	float						mHandBrake = 0.0f;

	bool m_update_config = false;


    template <class Archive>
    void save( Archive & ar ) const {
        LIL_SAVE_BASE(Actor);
		LIL_SER_FIELD(m_vehicle_config)
		LIL_SER_FIELD(mForward)
		LIL_SER_FIELD(mRight)
		LIL_SER_FIELD(mBrake)
		LIL_SER_FIELD(mHandBrake)
    }
       
    template <class Archive>
    void load( Archive & ar ) {
        LIL_LOAD_BASE(Actor)
		LIL_SER_FIELD(m_vehicle_config)
		LIL_SER_FIELD(mForward)
		LIL_SER_FIELD(mRight)
		LIL_SER_FIELD(mBrake)
		LIL_SER_FIELD(mHandBrake)
		ConstructVehicle(m_vehicle_config);
    }
};
LIL_REFLECT(Vehicle, bases<Actor>,
	field(m_vehicle_config),
	field(mForward),
	field(mRight),
	field(mBrake),
	field(mHandBrake),
	field(m_update_config)
)
LIL_SER_REGISTER_POLYMORPHIC(Vehicle)
LIL_DISAMBIGUATE_LOAD_SAVE(Vehicle)