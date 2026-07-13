#pragma once

#include "CommonIncludes.hpp"
#include <unordered_map>
#include <typeindex>

struct FieldInfo;
class Object;

template<typename T>
struct TypeResolver;

struct TypeInfo {
    const char* name = nullptr;
    size_t size = 0;
    const TypeInfo* parent = nullptr;
    Object* (*Create)() = nullptr;
    const std::vector<FieldInfo> fields;

    bool IsA(const TypeInfo* other) const {
        const TypeInfo* current = this;
        while(current) {
            if(current == other) return true;
            current = current->parent;
        }
        return false;
    }
};

class Object {
public:
    virtual const TypeInfo* GetType() const;
};

inline TypeInfo _Object_type {
    "Object",
    sizeof(Object),
    nullptr,
    []() -> Object* {return new Object();},
    {},
};

struct FieldInfo {
    const char* name;
    size_t offset;
    const TypeInfo* type;

    void* GetAddress(void* object) const {
        return (char*)object + offset;
    }

    const void* GetAddress(const void* object) const {
        return (const char*)object + offset;
    }
};

class TypeRegistry {
public:
    static TypeRegistry& Instance() {
        static TypeRegistry instance;
        return instance;
    }

    void Register(const TypeInfo* type) {
        m_types[type->name] = type;
    }

    const TypeInfo* Find(const char* name){
        auto it = m_types.find(name);
        if(it == m_types.end()) return nullptr;
        return it->second;
    }

private:
    std::unordered_map<std::string, const TypeInfo*> m_types;
};

#define PRIMITIVE_TYPE(typename) \
inline TypeInfo _##typename##_type { \
    #typename, \
    sizeof(typename), \
    nullptr, \
    nullptr, \
    {}\
};

#define COMPOSITE_TYPE(typename, ...) \
inline TypeInfo _##typename##_type { \
    #typename, \
    sizeof(typename), \
    nullptr, \
    nullptr, \
    __VA_ARGS__\
};

#define MEMBER_FIELD(parent, field_name) \
FieldInfo { \
    .name = #field_name, \
    .offset = offsetof(parent, field_name), \
    .type = TypeResolver<decltype(parent::field_name)>::Get() \
}

#define GENERATE_RESOLVER(typename) \
template<> \
struct TypeResolver<typename> { \
    static const TypeInfo* Get() { \
        return &_##typename##_type; \
    } \
}; \

#define PRIMITIVE(typename) \
PRIMITIVE_TYPE(typename)\
GENERATE_RESOLVER(typename)

#define COMPOSITE(typename, ...) \
COMPOSITE_TYPE(typename, __VA_ARGS__)\
GENERATE_RESOLVER(typename)\

#define CLASS_TYPE(classname, parent, ...) \
TypeInfo classname::s_type_info { \
    #classname, \
    sizeof(classname), \
    TypeResolver<parent>::Get(), \
    []() -> Object* {return new classname();}, \
    __VA_ARGS__ \
};

//put inside class
#define REFLECTABLE(classname) \
    static TypeInfo s_type_info;\
    virtual const TypeInfo* GetType() const override {\
        return &s_type_info;\
    }\

#define GENERATE_CLASS_RESOLVER(classname) \
template<> \
struct TypeResolver<classname> { \
    static const TypeInfo* Get() { \
        return &classname::s_type_info; \
    } \
}; \

// put after class
#define LIL_CLASS(classname, parent, ...) \
CLASS_TYPE(classname, parent, __VA_ARGS__) \
GENERATE_CLASS_RESOLVER(classname)

PRIMITIVE(bool)
PRIMITIVE(int)
PRIMITIVE(float)
PRIMITIVE(char)

inline TypeInfo _unsigned_char_type {
    "unsigned char",
    sizeof(unsigned char),
    nullptr,
    nullptr,
    {}
};
template<> struct TypeResolver<unsigned char> {
    static const TypeInfo* Get() { return &_unsigned_char_type; }
};

inline TypeInfo _std_string_type {
    "std::string",
    sizeof(std::string),
    nullptr,
    nullptr,
    {}
};
template<> struct TypeResolver<std::string> {
    static const TypeInfo* Get() { return &_std_string_type; }
};


COMPOSITE(Vector2, {
    MEMBER_FIELD(Vector2, x),
    MEMBER_FIELD(Vector2, y)
}
)

COMPOSITE(Vector3, {
    MEMBER_FIELD(Vector3, x),
    MEMBER_FIELD(Vector3, y),
    MEMBER_FIELD(Vector3, z)
}
)

COMPOSITE(Color, {
    MEMBER_FIELD(Color, r),
    MEMBER_FIELD(Color, g),
    MEMBER_FIELD(Color, b),
    MEMBER_FIELD(Color, a)
}
)

COMPOSITE(Quaternion, {
    MEMBER_FIELD(Quaternion, x),
    MEMBER_FIELD(Quaternion, y),
    MEMBER_FIELD(Quaternion, z),
    MEMBER_FIELD(Quaternion, w)
}
)

COMPOSITE(Transform, {
    MEMBER_FIELD(Transform, translation),
    MEMBER_FIELD(Transform, rotation),
    MEMBER_FIELD(Transform, scale),
}
)

template<> struct TypeResolver<Object> {
    static const TypeInfo* Get() {
        return &_Object_type;
    }
};