#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>

using FieldType = int;
// you can add your own type, starting from LilType::Unknown + 1
enum class LilType : FieldType {
    Bool,
    Int,
    Float,
    String,

    Vector2,
    Vector3,
    Quaternion,
    Color,

    Unknown
};

// inline LilType deduceLilType(auto* ptr) {
//     using T = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;
//     if (std::is_same_v<T, bool>) return LilType::Bool;
//     else if (std::is_same_v<T, int>) return LilType::Int;
//     else if (std::is_same_v<T, float>) return LilType::Float;
//     else if (std::is_same_v<T, std::string>) return LilType::String;

//     else if (std::is_same_v<T, Vector2>) return LilType::Vector2;
//     else if (std::is_same_v<T, Vector3>) return LilType::Vector3;
//     else if (std::is_same_v<T, Color>) return LilType::Color;

//     else return LilType::Unknown;
// }

struct Field {
    void* ptr = nullptr;
    FieldType type = FieldType(LilType::Unknown);
};

class Reflectable {
private:
    std::unordered_map<std::string, Field> m_fields;
public:
    Reflectable() {RegisterFields();}

    template <typename T>
    void AddField(const std::string& name, T* ptr, FieldType type) {
        m_fields[name] = Field{(void*)ptr, type};
    }

    template <typename T>
    void AddField(const std::string& name, T* ptr, LilType type) {
        m_fields[name] = Field{(void*)ptr, FieldType(type)};
    }

    virtual void RegisterFields() {};

    // template <typename T>
    // void AddField(const std::string& name, T* ptr) {
    //     AddField(name, ptr, deduceLilType(ptr));
    // }
};

//#define ADD_FIELD(serializer, var) serializer.AddField(NameFromObject(#var), &var)