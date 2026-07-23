#pragma once

#include "Reflection.hpp"

struct Attribute : refl::attr::usage::field, refl::attr::usage::function {}; 

struct ModelKeyAttribute : Attribute
{
};
REFL_AUTO(type(ModelKeyAttribute))

struct TextureKeyAttribute : Attribute
{
};
REFL_AUTO(type(TextureKeyAttribute))