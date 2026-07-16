#pragma once

#include "CommonIncludes.hpp"
#include "refl.hpp"

REFL_AUTO(
    type(Vector2),
    field(x),
    field(y)
)

REFL_AUTO(
    type(Vector3),
    field(x),
    field(y),
    field(z)
)

REFL_AUTO(
    type(Color),
    field(r),
    field(g),
    field(b),
    field(a)
)

REFL_AUTO(
    type(Vector4),
    field(x),
    field(y),
    field(z),
    field(w)
)

REFL_AUTO(
    type(Transform),
    field(translation),
    field(rotation),
    field(scale)
)

REFL_AUTO(
    type(Rectangle),
    field(x),
    field(y),
    field(width),
    field(height)
)