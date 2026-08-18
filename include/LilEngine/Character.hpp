#pragma once

#include "Actor.hpp"
#include "CommonIncludes.hpp"
#include "Physics.hpp"

class Character;

class CharacterContactAdapter : public JPH::CharacterContactListener {
public:
    Character* Owner = nullptr;
    virtual void OnContactAdded(const JPH::CharacterVirtual *inCharacter, const JPH::CharacterContact &inContact, JPH::CharacterContactSettings &ioSettings) override {};
};

class Character : public Actor {
public:
    Character();
    virtual ~Character();

    virtual void CharacterUpdate(float delta_time, bool jump, float jump_speed, float gravity, float move_x, float move_z);

    virtual void LayoutUpdate() override;
    virtual void SimulationUpdate(float delta_time) override;
    virtual void DebugDraw() override;

    bool IsOnGround() const;
    bool IsOnSteepSlope() const;
    Vector3 GetVelocity() const;
    Vector3 GetGroundNormal() const;
    Vector3 GetGroundVelocity() const;
    Vector3 GetUpVector() const;

    void SetVelocity(const Vector3 velocity);
    void SetHorizontalVelocity(const Vector3 direction, float speed);
    void AddImpulse(const Vector3 impulse);
    void Jump(float jump_speed);

private:

    CharacterContactAdapter mContactListener;
    JPH::Ref<JPH::CharacterVirtual> mCharacter;
    JPH::Ref<JPH::Shape> mStandingShape;
    JPH::BodyID mPhysicsPresenceID;

public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    float m_height = 2.0f;
    float m_radius = 1.0f;
};
LIL_REFLECT(Character, bases<Actor>)
LIL_SER_BEGIN(Character)
LIL_SER_BASE(Actor)
LIL_SER_FIELD(m_height)
LIL_SER_FIELD(m_radius)
LIL_SER_END()
