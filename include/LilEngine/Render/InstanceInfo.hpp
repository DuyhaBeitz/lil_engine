#pragma once
#include <raylib.h>
#include "Reflection.hpp"
#include "Serialization.hpp"

struct InstanceInfo : public Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    InstanceInfo() = default;
    InstanceInfo(Vector3 pos, Quaternion rot, Vector3 scal, Color col)
        : position(pos), rotation(rot), scale(scal), color(col) {}

    Vector3    position = {0, 0, 0};
    Quaternion rotation = {0, 0, 0, 1};
    Vector3    scale    = {1, 1, 1};
    Color      color    = {255, 255, 255, 255};
};
LIL_REFLECT(InstanceInfo, bases<>,
    field(position),
    field(rotation),
    field(scale),
    field(color)
)
LIL_SER_BEGIN(InstanceInfo)
LIL_SER_FIELD(position)
LIL_SER_FIELD(rotation)
LIL_SER_FIELD(scale)
LIL_SER_FIELD(color)
LIL_SER_END()