#pragma once

#include "Actor.hpp"
#include "CommonIncludes.hpp"
#include "Physics.hpp"
#include "ReflAttributes.hpp"

class Vehicle : public Actor {
private:
    JPH::BodyID m_body_id;
    JPH::Ref<JPH::VehicleConstraint> mVehicleConstraint;
    JPH::Ref<JPH::VehicleCollisionTester> mTester;

	virtual void DebugDraw() override;
    virtual void Draw() override;

public:
	std::string m_base_model_key = "None";
	std::string m_wheel_model_key = "None";

public:
    Vehicle();
    virtual ~Vehicle();

    void LayoutUpdate() override;
    void SimulationUpdate(float delta_time) override;



    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

private:
    static inline float			sInitialRollAngle = 0;
	static inline float			sMaxRollAngle = JPH::DegreesToRadians(60.0f);
	static inline float			sMaxSteeringAngle = JPH::DegreesToRadians(30.0f);
	static inline int			sCollisionMode = 2;
	static inline bool			sFourWheelDrive = false;
	static inline bool			sAntiRollbar = true;
	static inline bool			sLimitedSlipDifferentials = true;
	static inline bool			sOverrideGravity = false;					///< If true, gravity is overridden to always oppose the ground normal
	static inline float			sMaxEngineTorque = 500.0f;
	static inline float			sClutchStrength = 10.0f;
	static inline float			sFrontCasterAngle = 0.0f;
	static inline float			sFrontKingPinAngle = 0.0f;
	static inline float			sFrontCamber = 0.0f;
	static inline float			sFrontToe = 0.0f;
	static inline float			sFrontSuspensionForwardAngle = 0.0f;
	static inline float			sFrontSuspensionSidewaysAngle = 0.0f;
	static inline float			sFrontSuspensionMinLength = 0.3f;
	static inline float			sFrontSuspensionMaxLength = 0.5f;
	static inline float			sFrontSuspensionFrequency = 1.5f;
	static inline float			sFrontSuspensionDamping = 0.5f;
	static inline float			sRearSuspensionForwardAngle = 0.0f;
	static inline float			sRearSuspensionSidewaysAngle = 0.0f;
	static inline float			sRearCasterAngle = 0.0f;
	static inline float			sRearKingPinAngle = 0.0f;
	static inline float			sRearCamber = 0.0f;
	static inline float			sRearToe = 0.0f;
	static inline float			sRearSuspensionMinLength = 0.3f;
	static inline float			sRearSuspensionMaxLength = 0.5f;
	static inline float			sRearSuspensionFrequency = 1.5f;
	static inline float			sRearSuspensionDamping = 0.5f;


	// Player input
	float						mForward = 0.0f;
	float						mPreviousForward = 1.0f;					///< Keeps track of last car direction so we know when to brake and when to accelerate
	float						mRight = 0.0f;
	float						mBrake = 0.0f;
	float						mHandBrake = 0.0f;
};
LIL_REFLECT(Vehicle, bases<Actor>,
	field(m_base_model_key, ModelKeyAttribute{}),
	field(m_wheel_model_key, ModelKeyAttribute{})
)
LIL_SER_BEGIN(Vehicle)
LIL_SER_BASE(Actor)
LIL_SER_FIELD(m_base_model_key)
LIL_SER_FIELD(m_wheel_model_key)
LIL_SER_END()
