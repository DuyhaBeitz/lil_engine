#include "Vehicle.hpp"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "LilEngine.hpp"

// https://github.com/jrouwe/JoltPhysics/blob/master/Samples/Tests/Vehicle/VehicleConstraintTest.cpp

Vehicle::Vehicle() {
    const float wheel_radius = 0.3f;
	const float wheel_width = 0.1f;
	const float half_vehicle_length = 2.0f;
	const float half_vehicle_width = 0.9f;
	const float half_vehicle_height = 0.2f;

    // Create collision tester
	mTester = new JPH::VehicleCollisionTesterCastCylinder(JPH::Layers::MOVING);

	// Create vehicle body
	JPH::RVec3 position(0, 2, 0);
	JPH::RefConst<JPH::Shape> car_shape = JPH::OffsetCenterOfMassShapeSettings(
        JPH::Vec3(0, -half_vehicle_height, 0),
        new JPH::BoxShape(JPH::Vec3(half_vehicle_width, half_vehicle_height, half_vehicle_length))
    ).Create().Get();

	JPH::BodyCreationSettings car_body_settings(car_shape, position, JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), sInitialRollAngle), JPH::EMotionType::Dynamic, JPH::Layers::MOVING);
	car_body_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	car_body_settings.mMassPropertiesOverride.mMass = 1500.0f;
    JPH::BodyInterface& body_interface = Lil::Physics().GetBodyInterface();
    JPH::Body* CarBody = body_interface.CreateBody(car_body_settings);
	m_body_id = CarBody->GetID();
	body_interface.AddBody(CarBody->GetID(), JPH::EActivation::Activate);

	// Create vehicle constraint
	JPH::VehicleConstraintSettings vehicle;
	vehicle.mDrawConstraintSize = 0.1f;
	vehicle.mMaxPitchRollAngle = sMaxRollAngle;

	// Suspension direction
	JPH::Vec3 front_suspension_dir = JPH::Vec3(JPH::Tan(sFrontSuspensionSidewaysAngle), -1, JPH::Tan(sFrontSuspensionForwardAngle)).Normalized();
	JPH::Vec3 front_steering_axis = JPH::Vec3(-JPH::Tan(sFrontKingPinAngle), 1, -JPH::Tan(sFrontCasterAngle)).Normalized();
	JPH::Vec3 front_wheel_up = JPH::Vec3(JPH::Sin(sFrontCamber), JPH::Cos(sFrontCamber), 0);
	JPH::Vec3 front_wheel_forward = JPH::Vec3(-JPH::Sin(sFrontToe), 0, JPH::Cos(sFrontToe));
	JPH::Vec3 rear_suspension_dir = JPH::Vec3(JPH::Tan(sRearSuspensionSidewaysAngle), -1, JPH::Tan(sRearSuspensionForwardAngle)).Normalized();
	JPH::Vec3 rear_steering_axis = JPH::Vec3(-JPH::Tan(sRearKingPinAngle), 1, -JPH::Tan(sRearCasterAngle)).Normalized();
	JPH::Vec3 rear_wheel_up = JPH::Vec3(JPH::Sin(sRearCamber), JPH::Cos(sRearCamber), 0);
	JPH::Vec3 rear_wheel_forward = JPH::Vec3(-JPH::Sin(sRearToe), 0, JPH::Cos(sRearToe));
	JPH::Vec3 flip_x(-1, 1, 1);

    // Wheels, left front
	JPH::WheelSettingsWV *w1 = new JPH::WheelSettingsWV;
	w1->mPosition = JPH::Vec3(half_vehicle_width, -0.9f * half_vehicle_height, half_vehicle_length - 2.0f * wheel_radius);
	w1->mSuspensionDirection = front_suspension_dir;
	w1->mSteeringAxis = front_steering_axis;
	w1->mWheelUp = front_wheel_up;
	w1->mWheelForward = front_wheel_forward;
	w1->mSuspensionMinLength = sFrontSuspensionMinLength;
	w1->mSuspensionMaxLength = sFrontSuspensionMaxLength;
	w1->mSuspensionSpring.mFrequency = sFrontSuspensionFrequency;
	w1->mSuspensionSpring.mDamping = sFrontSuspensionDamping;
	w1->mMaxSteerAngle = sMaxSteeringAngle;
	w1->mMaxHandBrakeTorque = 0.0f; // Front wheel doesn't have hand brake

    // Right front
	JPH::WheelSettingsWV *w2 = new JPH::WheelSettingsWV;
	w2->mPosition = JPH::Vec3(-half_vehicle_width, -0.9f * half_vehicle_height, half_vehicle_length - 2.0f * wheel_radius);
	w2->mSuspensionDirection = flip_x * front_suspension_dir;
	w2->mSteeringAxis = flip_x * front_steering_axis;
	w2->mWheelUp = flip_x * front_wheel_up;
	w2->mWheelForward = flip_x * front_wheel_forward;
	w2->mSuspensionMinLength = sFrontSuspensionMinLength;
	w2->mSuspensionMaxLength = sFrontSuspensionMaxLength;
	w2->mSuspensionSpring.mFrequency = sFrontSuspensionFrequency;
	w2->mSuspensionSpring.mDamping = sFrontSuspensionDamping;
	w2->mMaxSteerAngle = sMaxSteeringAngle;
	w2->mMaxHandBrakeTorque = 0.0f; // Front wheel doesn't have hand brake

    // Left rear
	JPH::WheelSettingsWV *w3 = new JPH::WheelSettingsWV;
	w3->mPosition = JPH::Vec3(half_vehicle_width, -0.9f * half_vehicle_height, -half_vehicle_length + 2.0f * wheel_radius);
	w3->mSuspensionDirection = rear_suspension_dir;
	w3->mSteeringAxis = rear_steering_axis;
	w3->mWheelUp = rear_wheel_up;
	w3->mWheelForward = rear_wheel_forward;
	w3->mSuspensionMinLength = sRearSuspensionMinLength;
	w3->mSuspensionMaxLength = sRearSuspensionMaxLength;
	w3->mSuspensionSpring.mFrequency = sRearSuspensionFrequency;
	w3->mSuspensionSpring.mDamping = sRearSuspensionDamping;
	w3->mMaxSteerAngle = 0.0f;

    // Right rear
	JPH::WheelSettingsWV *w4 = new JPH::WheelSettingsWV;
	w4->mPosition = JPH::Vec3(-half_vehicle_width, -0.9f * half_vehicle_height, -half_vehicle_length + 2.0f * wheel_radius);
	w4->mSuspensionDirection = flip_x * rear_suspension_dir;
	w4->mSteeringAxis = flip_x * rear_steering_axis;
	w4->mWheelUp = flip_x * rear_wheel_up;
	w4->mWheelForward = flip_x * rear_wheel_forward;
	w4->mSuspensionMinLength = sRearSuspensionMinLength;
	w4->mSuspensionMaxLength = sRearSuspensionMaxLength;
	w4->mSuspensionSpring.mFrequency = sRearSuspensionFrequency;
	w4->mSuspensionSpring.mDamping = sRearSuspensionDamping;
	w4->mMaxSteerAngle = 0.0f;

    vehicle.mWheels = { w1, w2, w3, w4 };
    for (JPH::WheelSettings *w : vehicle.mWheels) {
		w->mRadius = wheel_radius;
		w->mWidth = wheel_width;
	}

	JPH::WheeledVehicleControllerSettings *controller = new JPH::WheeledVehicleControllerSettings;
	vehicle.mController = controller;

	// Differential
	controller->mDifferentials.resize(sFourWheelDrive? 2 : 1);
	controller->mDifferentials[0].mLeftWheel = 0;
	controller->mDifferentials[0].mRightWheel = 1;
	if (sFourWheelDrive){
		controller->mDifferentials[1].mLeftWheel = 2;
		controller->mDifferentials[1].mRightWheel = 3;

		// Split engine torque
		controller->mDifferentials[0].mEngineTorqueRatio = controller->mDifferentials[1].mEngineTorqueRatio = 0.5f;
	}

    // Anti rollbars
	if (sAntiRollbar) {
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
	controller->GetEngine().mMaxTorque = sMaxEngineTorque;
	controller->GetTransmission().mClutchStrength = sClutchStrength;

	// Set slip ratios to the same for everything
	float limited_slip_ratio = sLimitedSlipDifferentials? 1.4f : FLT_MAX;
	controller->SetDifferentialLimitedSlipRatio(limited_slip_ratio);
	for (JPH::VehicleDifferentialSettings &d : controller->GetDifferentials())
		d.mLimitedSlipRatio = limited_slip_ratio;

	// Pass the input on to the constraint
	controller->SetDriverInput(mForward, mRight, mBrake, mHandBrake);

	JPH::Vec3 pos = bodyInterface.GetPosition(m_body_id);
	JPH::Quat rot = bodyInterface.GetRotation(m_body_id);

	if (sOverrideGravity) {
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

void Vehicle::DebugDraw() {
	Actor::DebugDraw();

	for (uint w = 0; w < 4; ++w) {
		const JPH::WheelSettings *settings = mVehicleConstraint->GetWheels()[w]->GetSettings();
		JPH::RMat44 wheel_transform = mVehicleConstraint->GetWheelWorldTransform(w, JPH::Vec3::sAxisY(), JPH::Vec3::sAxisX()); // The cylinder we draw is aligned with Y so we specify that as rotational axis
		Lil::Physics().GetDebugRender()->DrawCylinder(wheel_transform, 0.5f * settings->mWidth, settings->mRadius, JPH::Color::sGreen);
	}
}

void Vehicle::Draw() {
	Actor::Draw();

	for (uint w = 0; w < 4; ++w) {
		if (R3D_Model* m = Lil::Resources().GetModel(m_wheel_model_key)) {
			const JPH::WheelSettings *settings = mVehicleConstraint->GetWheels()[w]->GetSettings();
			JPH::RMat44 wheel_transform = mVehicleConstraint->GetWheelWorldTransform(w, JPH::Vec3::sAxisY(), JPH::Vec3::sAxisX()); // The cylinder we draw is aligned with Y so we specify that as rotational axis

			JPH::Vec3 position = wheel_transform.GetTranslation();
			JPH::Quat rotation = wheel_transform.GetRotation().GetQuaternion();

			R3D_DrawModelEx(*m, RlVector3(position), RlQuat(rotation), Vector3{1.0f, 1.0f, 1.0f});
		}
	}
}
