#pragma once

#include "CommonIncludes.hpp"
#include "Reflection.hpp"

class Environment : public Reflectable {
public:
    Environment() = default;

    LIL_REFLECTABLE()

    
};
LIL_REFLECT(Environment, bases<>
)