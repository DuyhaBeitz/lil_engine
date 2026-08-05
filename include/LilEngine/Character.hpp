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
    ~Character();

    virtual void CharacterUpdate(float delta_time) {
        Vector3 vel = GetVelocity();
        bool jump = false;
        float jump_speed = 6.0f;
        float gravity = -9.81;
        float move_x = 0.0f;
        float move_z = 0.0f;

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

    void OnLayoutUpdate() override;
    void OnSimulationUpdate(float delta_time) override;

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
