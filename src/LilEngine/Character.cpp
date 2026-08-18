#include "Character.hpp"
#include "LilEngine.hpp"
#include "utils/ColliderHelper.hpp"

Character::Character(float height, float radius) 
: m_height(height), m_radius(radius)
{
    mContactListener.Owner = this;

    mStandingShape = new JPH::CapsuleShape(m_height * 0.5f, m_radius);

    JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
    settings->mShape = mStandingShape;
    settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f); // Max walkable slope angle
    settings->mMaxStrength = 100.0f;                         // Max force used to push rigid bodies
    settings->mBackFaceMode = JPH::EBackFaceMode::IgnoreBackFaces;
    settings->mCharacterPadding = 0.02f;                      // Small buffer to prevent sticking to walls
    settings->mPenetrationRecoverySpeed = 1.0f;
    settings->mPredictiveContactDistance = 0.1f;
    
    // supporting volume (bottom sphere of the capsule) for slope checks
    settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -m_radius);

    mCharacter = new JPH::CharacterVirtual(
        settings, 
        JPH::Vec3(0.0f, 0.0f, 0.0f), 
        JPH::Quat::sIdentity(), 
        0,
        Lil::Physics().GetSystem()
    );
    mCharacter->SetListener(&mContactListener);

    JPH::BodyCreationSettings bodySettings(
        mStandingShape,
        JPH::RVec3::sZero(),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Kinematic,
        JPH::Layers::MOVING
    );
    

    JPH::BodyInterface& bodyInterface = Lil::Physics().GetSystem()->GetBodyInterface();
    JPH::Body* body = bodyInterface.CreateBody(bodySettings);
    
    if (body) {
        mPhysicsPresenceID = body->GetID();
        bodyInterface.AddBody(mPhysicsPresenceID, JPH::EActivation::Activate);
    }
}

Character::~Character() {
    if (!mPhysicsPresenceID.IsInvalid()) {
        Lil::Physics().GetSystem()->GetBodyInterface().RemoveBody(mPhysicsPresenceID);
        Lil::Physics().GetSystem()->GetBodyInterface().DestroyBody(mPhysicsPresenceID);
        mPhysicsPresenceID = JPH::BodyID();
    }
}

bool Character::IsOnGround() const { return mCharacter->IsSupported(); }
bool Character::IsOnSteepSlope() const { return mCharacter->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnSteepGround; }
Vector3 Character::GetVelocity() const { return RlVector3(mCharacter->GetLinearVelocity()); }
Vector3 Character::GetGroundNormal() const { return RlVector3(mCharacter->GetGroundNormal()); }
Vector3 Character::GetGroundVelocity() const { return RlVector3(mCharacter->GetGroundVelocity()); }
Vector3 Character::GetUpVector() const { return RlVector3(mCharacter->GetUp()); }

void Character::SetVelocity(const Vector3 velocity) { mCharacter->SetLinearVelocity(JphVector3(velocity));}
void Character::SetHorizontalVelocity(const Vector3 direction, float speed) {
    const Vector3 norm_dir = Vector3Normalize(direction);
    SetVelocity(Vector3{
        norm_dir.x * speed,
        GetVelocity().y,
        norm_dir.z * speed        
    });
}
void Character::AddImpulse(const Vector3 impulse) {
    mCharacter->SetLinearVelocity(mCharacter->GetLinearVelocity() + JphVector3(impulse));
}
void Character::Jump(float jump_speed) {
    AddImpulse(GetUpVector()*jump_speed);
}

void Character::CharacterUpdate(float delta_time, bool jump, float jump_speed, float gravity, float move_x, float move_z) {
    Vector3 vel = GetVelocity();

    if (IsOnGround()) {
        if (jump) vel.y = jump_speed + GetGroundVelocity().y; // or call Jump(jump_speed);
        else vel.y = GetGroundVelocity().y;

        float speed = 100.0f;
        vel.x = GetGroundVelocity().x + delta_time * speed * move_x;
        vel.z = GetGroundVelocity().z + delta_time * speed * move_z;

    } else {
        vel.y += gravity * delta_time;
    }

    SetVelocity(vel);
}

void Character::LayoutUpdate()
{
    Actor::LayoutUpdate();
    
    mCharacter->SetPosition(JphVector3(GetPosition()));
    mCharacter->SetRotation(JphQuat(GetRotation()));

    if (!mPhysicsPresenceID.IsInvalid()) {
        JPH::BodyInterface& bodyInterface = Lil::Physics().GetSystem()->GetBodyInterface();
        bodyInterface.SetPositionAndRotation(
            mPhysicsPresenceID, 
            mCharacter->GetPosition(), 
            mCharacter->GetRotation(), 
            JPH::EActivation::DontActivate
        );
    }
};

void Character::SimulationUpdate(float delta_time) {
    Actor::SimulationUpdate(delta_time);

    JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
    JPH::DefaultBroadPhaseLayerFilter broadphaseFilter = Lil::Physics().GetSystem()->GetDefaultBroadPhaseLayerFilter(JPH::Layers::MOVING);
    JPH::DefaultObjectLayerFilter objectFilter = Lil::Physics().GetSystem()->GetDefaultLayerFilter(JPH::Layers::MOVING);
    
    JPH::IgnoreSingleBodyFilter bodyFilter(mPhysicsPresenceID);
    mCharacter->ExtendedUpdate(
        delta_time,
        -mCharacter->GetUp() * Lil::Physics().GetSystem()->GetGravity().Length(), // Extra gravity push for sweeping
        updateSettings,
        broadphaseFilter,
        objectFilter,
        bodyFilter,
        {}, // ShapeFilter
        *Lil::Physics().GetTempAllocator()
    );

    SetPosition(RlVector3(mCharacter->GetPosition()));
    SetRotation(RlQuat(mCharacter->GetRotation()));

    if (!mPhysicsPresenceID.IsInvalid()) {
        JPH::BodyInterface& bodyInterface = Lil::Physics().GetSystem()->GetBodyInterface();
        bodyInterface.SetPositionAndRotation(
            mPhysicsPresenceID, 
            mCharacter->GetPosition(), 
            mCharacter->GetRotation(), 
            JPH::EActivation::DontActivate
        );
    }
}

void Character::DebugDraw() {
    Actor::DebugDraw();

    DrawDebugPhysicsBody(mPhysicsPresenceID);
}