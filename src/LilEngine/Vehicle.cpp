#include "Vehicle.hpp"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "utils/ColliderHelper.hpp"

#include "LilEngine.hpp"

// https://github.com/jrouwe/JoltPhysics/blob/master/Samples/Tests/Vehicle/VehicleConstraintTest.cpp

Vehicle::Vehicle(const VehicleConfig& config) 
: m_vehicle_config(config)
{
    // Create collision tester
	mTester = new JPH::VehicleCollisionTesterCastCylinder(JPH::Layers::MOVING);

	// Create vehicle body
	JPH::RVec3 position(0, 2, 0);
	JPH::RefConst<JPH::Shape> car_shape = JPH::OffsetCenterOfMassShapeSettings(
        JPH::Vec3(0, -m_vehicle_config.half_vehicle_height, 0),
        new JPH::BoxShape(JPH::Vec3(m_vehicle_config.half_vehicle_width, m_vehicle_config.half_vehicle_height, m_vehicle_config.half_vehicle_length))
    ).Create().Get();

	JPH::BodyCreationSettings car_body_settings(car_shape, position, JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), m_vehicle_config.InitialRollAngle), JPH::EMotionType::Dynamic, JPH::Layers::MOVING);
	car_body_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	car_body_settings.mMassPropertiesOverride.mMass = 1500.0f;
    JPH::BodyInterface& body_interface = Lil::Physics().GetBodyInterface();
    JPH::Body* CarBody = body_interface.CreateBody(car_body_settings);
	m_body_id = CarBody->GetID();
	body_interface.AddBody(CarBody->GetID(), JPH::EActivation::Activate);

	// Create vehicle constraint
	JPH::VehicleConstraintSettings vehicle;
	vehicle.mDrawConstraintSize = 0.1f;
	vehicle.mMaxPitchRollAngle = m_vehicle_config.MaxRollAngle;

	// Suspension direction
	JPH::Vec3 front_suspension_dir = JPH::Vec3(JPH::Tan(m_vehicle_config.FrontSuspensionSidewaysAngle), -1, JPH::Tan(m_vehicle_config.FrontSuspensionForwardAngle)).Normalized();
	JPH::Vec3 front_steering_axis = JPH::Vec3(-JPH::Tan(m_vehicle_config.FrontKingPinAngle), 1, -JPH::Tan(m_vehicle_config.FrontCasterAngle)).Normalized();
	JPH::Vec3 front_wheel_up = JPH::Vec3(JPH::Sin(m_vehicle_config.FrontCamber), JPH::Cos(m_vehicle_config.FrontCamber), 0);
	JPH::Vec3 front_wheel_forward = JPH::Vec3(-JPH::Sin(m_vehicle_config.FrontToe), 0, JPH::Cos(m_vehicle_config.FrontToe));
	JPH::Vec3 rear_suspension_dir = JPH::Vec3(JPH::Tan(m_vehicle_config.RearSuspensionSidewaysAngle), -1, JPH::Tan(m_vehicle_config.RearSuspensionForwardAngle)).Normalized();
	JPH::Vec3 rear_steering_axis = JPH::Vec3(-JPH::Tan(m_vehicle_config.RearKingPinAngle), 1, -JPH::Tan(m_vehicle_config.RearCasterAngle)).Normalized();
	JPH::Vec3 rear_wheel_up = JPH::Vec3(JPH::Sin(m_vehicle_config.RearCamber), JPH::Cos(m_vehicle_config.RearCamber), 0);
	JPH::Vec3 rear_wheel_forward = JPH::Vec3(-JPH::Sin(m_vehicle_config.RearToe), 0, JPH::Cos(m_vehicle_config.RearToe));
	JPH::Vec3 flip_x(-1, 1, 1);

    // Wheels, left front
	JPH::WheelSettingsWV *w1 = new JPH::WheelSettingsWV;
	w1->mPosition = JPH::Vec3(m_vehicle_config.half_vehicle_width, -0.9f * m_vehicle_config.half_vehicle_height, m_vehicle_config.half_vehicle_length - 2.0f * m_vehicle_config.wheel_radius);
	w1->mSuspensionDirection = front_suspension_dir;
	w1->mSteeringAxis = front_steering_axis;
	w1->mWheelUp = front_wheel_up;
	w1->mWheelForward = front_wheel_forward;
	w1->mSuspensionMinLength = m_vehicle_config.FrontSuspensionMinLength;
	w1->mSuspensionMaxLength = m_vehicle_config.FrontSuspensionMaxLength;
	w1->mSuspensionSpring.mFrequency = m_vehicle_config.FrontSuspensionFrequency;
	w1->mSuspensionSpring.mDamping = m_vehicle_config.FrontSuspensionDamping;
	w1->mMaxSteerAngle = m_vehicle_config.MaxSteeringAngle;
	w1->mMaxHandBrakeTorque = 0.0f; // Front wheel doesn't have hand brake

    // Right front
	JPH::WheelSettingsWV *w2 = new JPH::WheelSettingsWV;
	w2->mPosition = JPH::Vec3(-m_vehicle_config.half_vehicle_width, -0.9f * m_vehicle_config.half_vehicle_height, m_vehicle_config.half_vehicle_length - 2.0f * m_vehicle_config.wheel_radius);
	w2->mSuspensionDirection = flip_x * front_suspension_dir;
	w2->mSteeringAxis = flip_x * front_steering_axis;
	w2->mWheelUp = flip_x * front_wheel_up;
	w2->mWheelForward = flip_x * front_wheel_forward;
	w2->mSuspensionMinLength = m_vehicle_config.FrontSuspensionMinLength;
	w2->mSuspensionMaxLength = m_vehicle_config.FrontSuspensionMaxLength;
	w2->mSuspensionSpring.mFrequency = m_vehicle_config.FrontSuspensionFrequency;
	w2->mSuspensionSpring.mDamping = m_vehicle_config.FrontSuspensionDamping;
	w2->mMaxSteerAngle = m_vehicle_config.MaxSteeringAngle;
	w2->mMaxHandBrakeTorque = 0.0f; // Front wheel doesn't have hand brake

    // Left rear
	JPH::WheelSettingsWV *w3 = new JPH::WheelSettingsWV;
	w3->mPosition = JPH::Vec3(m_vehicle_config.half_vehicle_width, -0.9f * m_vehicle_config.half_vehicle_height, -m_vehicle_config.half_vehicle_length + 2.0f * m_vehicle_config.wheel_radius);
	w3->mSuspensionDirection = rear_suspension_dir;
	w3->mSteeringAxis = rear_steering_axis;
	w3->mWheelUp = rear_wheel_up;
	w3->mWheelForward = rear_wheel_forward;
	w3->mSuspensionMinLength = m_vehicle_config.RearSuspensionMinLength;
	w3->mSuspensionMaxLength = m_vehicle_config.RearSuspensionMaxLength;
	w3->mSuspensionSpring.mFrequency = m_vehicle_config.RearSuspensionFrequency;
	w3->mSuspensionSpring.mDamping = m_vehicle_config.RearSuspensionDamping;
	w3->mMaxSteerAngle = 0.0f;

    // Right rear
	JPH::WheelSettingsWV *w4 = new JPH::WheelSettingsWV;
	w4->mPosition = JPH::Vec3(-m_vehicle_config.half_vehicle_width, -0.9f * m_vehicle_config.half_vehicle_height, -m_vehicle_config.half_vehicle_length + 2.0f * m_vehicle_config.wheel_radius);
	w4->mSuspensionDirection = flip_x * rear_suspension_dir;
	w4->mSteeringAxis = flip_x * rear_steering_axis;
	w4->mWheelUp = flip_x * rear_wheel_up;
	w4->mWheelForward = flip_x * rear_wheel_forward;
	w4->mSuspensionMinLength = m_vehicle_config.RearSuspensionMinLength;
	w4->mSuspensionMaxLength = m_vehicle_config.RearSuspensionMaxLength;
	w4->mSuspensionSpring.mFrequency = m_vehicle_config.RearSuspensionFrequency;
	w4->mSuspensionSpring.mDamping = m_vehicle_config.RearSuspensionDamping;
	w4->mMaxSteerAngle = 0.0f;

    vehicle.mWheels = { w1, w2, w3, w4 };
    for (JPH::WheelSettings *w : vehicle.mWheels) {
		w->mRadius = m_vehicle_config.wheel_radius;
		w->mWidth = m_vehicle_config.wheel_width;
	}

	JPH::WheeledVehicleControllerSettings *controller = new JPH::WheeledVehicleControllerSettings;
	vehicle.mController = controller;

	// Differential
	controller->mDifferentials.resize(m_vehicle_config.FourWheelDrive? 2 : 1);
	controller->mDifferentials[0].mLeftWheel = 0;
	controller->mDifferentials[0].mRightWheel = 1;
	if (m_vehicle_config.FourWheelDrive){
		controller->mDifferentials[1].mLeftWheel = 2;
		controller->mDifferentials[1].mRightWheel = 3;

		// Split engine torque
		controller->mDifferentials[0].mEngineTorqueRatio = controller->mDifferentials[1].mEngineTorqueRatio = 0.5f;
	}

    // Anti rollbars
	if (m_vehicle_config.AntiRollbar) {
		vehicle.mAntiRollBars.resize(2);
		vehicle.mAntiRollBars[0].mLeftWheel = 0;
		vehicle.mAntiRollBars[0].mRightWheel = 1;
		vehicle.mAntiRollBars[1].mLeftWheel = 2;
		vehicle.mAntiRollBars[1].mRightWheel = 3;
	}

	mVehicleConstraint = new JPH::VehicleConstraint(*CarBody, vehicle);
	mVehicleConstraint->SetVehicleCollisionTester(mTester);

	// The vehicle settings were tweaked with a buggy implementation of the longitudinal tire impulses, this meant that PhysicsSettings::mNumVelocitySteps times more impulse
	// could be applied than intended. To keep the behavior of the vehicle the same we increase the max longitudinal impulse by the same factor. In a future version the vehicle
	// will be retweaked.
	static_cast<JPH::WheeledVehicleController *>(mVehicleConstraint->GetController())->SetTireMaxImpulseCallback(
		[](JPH::uint, float &outLongitudinalImpulse, float &outLateralImpulse, float inSuspensionImpulse, float inLongitudinalFriction, float inLateralFriction, float, float, float)
		{
			outLongitudinalImpulse = 10.0f * inLongitudinalFriction * inSuspensionImpulse;
			outLateralImpulse = inLateralFriction * inSuspensionImpulse;
		});

    Lil::Physics().GetSystem()->AddConstraint(mVehicleConstraint);
	Lil::Physics().GetSystem()->AddStepListener(mVehicleConstraint);
}

Vehicle::~Vehicle() {
	Lil::Physics().GetSystem()->RemoveStepListener(mVehicleConstraint);
    Lil::Physics().GetSystem()->RemoveConstraint(mVehicleConstraint);
	
    if (!m_body_id.IsInvalid()) {
        Lil::Physics().GetSystem()->GetBodyInterface().RemoveBody(m_body_id);
        Lil::Physics().GetSystem()->GetBodyInterface().DestroyBody(m_body_id);
        m_body_id = JPH::BodyID();
    }
}

void Vehicle::LayoutUpdate() {
	Actor::LayoutUpdate();

    if (!m_body_id.IsInvalid()) {
        JPH::BodyInterface& bodyInterface = Lil::Physics().GetSystem()->GetBodyInterface();
        bodyInterface.SetPositionAndRotation(
            m_body_id, 
            JphVector3(GetPosition()),
            JphQuat(GetRotation()),
            JPH::EActivation::DontActivate
        );
    }
}

void Vehicle::SimulationUpdate(float delta_time) {
	Actor::SimulationUpdate(delta_time);

	JPH::PhysicsSystem* physicsSystem = Lil::Physics().GetSystem();
	JPH::BodyInterface& bodyInterface = physicsSystem->GetBodyInterface();

	// On user input, assure that the car is active
	if (mRight != 0.0f || mForward != 0.0f || mBrake != 0.0f || mHandBrake != 0.0f)
		bodyInterface.ActivateBody(m_body_id);

	JPH::WheeledVehicleController *controller = static_cast<JPH::WheeledVehicleController *>(mVehicleConstraint->GetController());

	// Update vehicle statistics
	controller->GetEngine().mMaxTorque = m_vehicle_config.MaxEngineTorque;
	controller->GetTransmission().mClutchStrength = m_vehicle_config.ClutchStrength;

	// Set slip ratios to the same for everything
	float limited_slip_ratio = m_vehicle_config.LimitedSlipDifferentials? 1.4f : FLT_MAX;
	controller->SetDifferentialLimitedSlipRatio(limited_slip_ratio);
	for (JPH::VehicleDifferentialSettings &d : controller->GetDifferentials())
		d.mLimitedSlipRatio = limited_slip_ratio;

	// Pass the input on to the constraint
	controller->SetDriverInput(mForward, mRight, mBrake, mHandBrake);

	JPH::Vec3 pos = bodyInterface.GetPosition(m_body_id);
	JPH::Quat rot = bodyInterface.GetRotation(m_body_id);

	if (m_vehicle_config.OverrideGravity) {
		// When overriding gravity is requested, we cast a sphere downwards (opposite to the previous up position) and use the contact normal as the new gravity direction
		JPH::SphereShape sphere(0.5f);
		sphere.SetEmbedded();
		JPH::RShapeCast shape_cast(&sphere, JPH::Vec3::sOne(), JPH::RMat44::sTranslation(pos), -3.0f * mVehicleConstraint->GetWorldUp());
		JPH::ShapeCastSettings settings;
		JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;
		physicsSystem->GetNarrowPhaseQuery().CastShape(shape_cast, settings, pos, collector, JPH::SpecifiedBroadPhaseLayerFilter(JPH::BroadPhaseLayers::NON_MOVING), JPH::SpecifiedObjectLayerFilter(JPH::Layers::NON_MOVING));
		if (collector.HadHit())
			mVehicleConstraint->OverrideGravity(9.81f * collector.mHit.mPenetrationAxis.Normalized());
		else
			mVehicleConstraint->ResetGravityOverride();
	}

	SetPosition(RlVector3(pos));
	SetRotation(RlQuat(rot));
}

std::array<Transform, 4> Vehicle::GetWheelTransforms() {
	std::array<Transform, 4> wheel_transforms;

	for (uint w = 0; w < 4; ++w) {
		const JPH::WheelSettings *settings = mVehicleConstraint->GetWheels()[w]->GetSettings();
		JPH::RMat44 wheel_transform = mVehicleConstraint->GetWheelWorldTransform(w, JPH::Vec3::sAxisY(), JPH::Vec3::sAxisX()); // The cylinder we draw is aligned with Y so we specify that as rotational axis
		JPH::Vec3 position = wheel_transform.GetTranslation();
		JPH::Quat rotation = wheel_transform.GetRotation().GetQuaternion();
		wheel_transforms[w] = Transform{.translation = RlVector3(position), .rotation = RlQuat(rotation), .scale = Vector3{1.0f, 1.0f, 1.0f}};
	}
	return wheel_transforms;
}

void Vehicle::DebugDraw() {
	Actor::DebugDraw();

	for (uint w = 0; w < 4; ++w) {
		const JPH::WheelSettings *settings = mVehicleConstraint->GetWheels()[w]->GetSettings();
		JPH::RMat44 wheel_transform = mVehicleConstraint->GetWheelWorldTransform(w, JPH::Vec3::sAxisY(), JPH::Vec3::sAxisX()); // The cylinder we draw is aligned with Y so we specify that as rotational axis
		Lil::Physics().GetDebugRender()->DrawCylinder(wheel_transform, 0.5f * settings->mWidth, settings->mRadius, JPH::Color::sGreen);
	}
	DrawDebugPhysicsBody(m_body_id);
}