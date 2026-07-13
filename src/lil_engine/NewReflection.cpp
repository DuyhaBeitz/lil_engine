#include "NewReflection.hpp"

const TypeInfo *Object::GetType() const { return &_Object_type; }