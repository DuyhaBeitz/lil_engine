#pragma once

#include "Pawn.hpp"

// Character is a Pawn that only rotates around Z axis and has handy controls
class Character : public Pawn {
public:
    Character() : Pawn(rc::BodyType::DYNAMIC) {
        SetAngularLockAxisFactor(Vector3{0, 1, 0});
    }
};