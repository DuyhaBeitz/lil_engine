#include "NewReflection.hpp"
#include <iostream>

#define GENERATE_PRINT(typename)  \
if (type->name == #typename) \
{std::cout << " = " << *static_cast<typename*>(obj) << std::endl; return;}

void Print(const TypeInfo* type, void* obj, int depth = 1, const char* name = "", bool last_field = false, std::string ident = "") {
    if (last_field)  std::cout << "└─ ";
    else std::cout << "├─ ";
    std::cout << type->name << " " << name;
    GENERATE_PRINT(bool)
    GENERATE_PRINT(int)
    GENERATE_PRINT(float)
    
    Object* o = static_cast<Object*>(obj);
    std::cout << std::endl;
    for (int i = 0; i < type->fields.size(); i++) {
        auto field = type->fields[i];
        std::cout << ident;

        std::string ident_add = "   ";
        if (i != type->fields.size()-1) ident_add = "│  ";
        Print(field.type, field.GetAddress(obj), depth+1, field.name, i == type->fields.size()-1, ident + ident_add);
    }
}