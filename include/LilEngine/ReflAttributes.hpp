#pragma once

#include "Reflection.hpp"

struct Attribute : refl::attr::usage::field, refl::attr::usage::function {}; 

struct ModelKeyAttribute : Attribute
{
};

struct TextureKeyAttribute : Attribute
{
};