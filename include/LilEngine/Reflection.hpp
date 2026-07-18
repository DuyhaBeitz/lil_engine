#pragma once

#include "CommonIncludes.hpp"

#include <vector>
#include <set>
#include <cassert>
#include <iostream>
#include <raylib.h>
#include "refl.hpp"

class TypeInfo;

struct FieldInfo {
    std::string parent_type_name;
    std::string name;
    const TypeInfo& type;

    void* (*GetPtr)(void*);
    const void* (*GetPtrConst)(const void*);

    FieldInfo(const TypeInfo& type_) : type(type_) {}
};

class IFieldVisitor {
public:
    virtual ~IFieldVisitor() = default;
    virtual bool Visit(const FieldInfo& field, void* instance) = 0;
};

class TypeInfo {
public:

    template<typename T>
    static const TypeInfo& Get() {
        static const TypeInfo info(refl::reflect<T>());
        return info;
    }

    const std::string& Name() const {
        return m_name;
    }

    const std::vector<FieldInfo>& Fields() const {
        return m_fields;
    }

    const std::vector<const TypeInfo*>& Bases() const {
        return m_bases;
    }


    template<typename T>
    bool IsA() const {
        return IsA_Internal<T>(this);
    }

    void VisitFields(void* instance, IFieldVisitor& visitor) const {
        for (const auto& field : m_fields) {
            if (!visitor.Visit(field, field.GetPtr(instance))) break;
        }
    }

    bool operator==(const TypeInfo& other) const {
        return m_name == other.Name();
    }

private:

    std::string m_name;
    std::vector<FieldInfo> m_fields;
    std::vector<const TypeInfo*> m_bases;

    template<typename T>
    TypeInfo(refl::type_descriptor<T> td) : m_name(td.name) {
        // populate bases
        constexpr auto type = refl::reflect<T>();
        if constexpr (type.declared_bases.size) {
            refl::util::for_each(reflect_types(type.declared_bases), [this](auto t) {
                using BaseType = typename decltype(t)::type;
                m_bases.push_back(&TypeInfo::Get<BaseType>());
            });
        }
        
        refl::util::for_each(td.members, [this, td](auto member) {
            using Member = decltype(member);

            // populate fields
            if constexpr (refl::descriptor::is_field(Member{})) {
                using FieldType = typename Member::value_type;

                FieldInfo info(TypeInfo::Get<FieldType>());

                info.name = member.name.c_str();
                info.parent_type_name = m_name;

                info.GetPtr = [](void* object) -> void* {
                    auto* obj = static_cast<T*>(object);
                    return &(obj->*Member::pointer);
                };

                info.GetPtrConst = [](const void* object) -> const void* {
                    auto* obj = static_cast<const T*>(object);
                    return &(obj->*Member::pointer);
                };

                m_fields.push_back(std::move(info));
            }
        });
    }

    template<typename T>
    bool IsA_Internal(const TypeInfo* current) const {
        if (!current) return false;
        
        if (current == &TypeInfo::Get<T>()) return true;
        
        for (const auto* base : current->m_bases) {
            if (IsA_Internal<T>(base)) {
                return true;
            }
        }
        
        return false;
    }
};


namespace Lil {
    class Reflection {
    public:
        static Reflection& Get() {
            static Reflection instance;
            return instance;
        }

        template<typename T>
        void RegisterType() {
            m_types.insert(&TypeInfo::Get<T>());
        }

        std::set<const TypeInfo*>& Types() { return m_types; }
               
    private:
        std::set<const TypeInfo*> m_types;
    };
};

template<typename T>
struct TypeRegisterer {
    TypeRegisterer() {
        Lil::Reflection().Get().RegisterType<T>();
    }
};

class Reflectable {
public:
    virtual const TypeInfo& GetTypeInfo() const = 0;
};

// define a convenience macro to autoimplement GetTypeInfo()
#define LIL_REFLECTABLE() \
    virtual const TypeInfo& GetTypeInfo() const override \
    { \
        return TypeInfo::Get<::refl::trait::remove_qualifiers_t<decltype(*this)>>(); \
    }

#define LIL_REFLECT_NO_FIELDS(typename, bases) \
REFL_AUTO(type(typename, bases)) \
static TypeRegisterer<typename> _##typename##_registerer;

#define LIL_REFLECT_NO_BASE(typename, ...) \
REFL_AUTO(type(typename), __VA_ARGS__) \
static TypeRegisterer<typename> _##typename##_registerer;

#define LIL_REFLECT(typename, bases, ...) \
REFL_AUTO(type(typename, bases), __VA_ARGS__) \
static TypeRegisterer<typename> _##typename##_registerer;


LIL_REFLECT_NO_BASE(
    Vector2,
    field(x),
    field(y)
)

LIL_REFLECT_NO_BASE(
    Vector3,
    field(x),
    field(y),
    field(z)
)

LIL_REFLECT_NO_BASE(
    Color,
    field(r),
    field(g),
    field(b),
    field(a)
)

LIL_REFLECT_NO_BASE(
    Vector4,
    field(x),
    field(y),
    field(z),
    field(w)
)

LIL_REFLECT_NO_BASE(
    Transform,
    field(translation),
    field(rotation),
    field(scale)
)

LIL_REFLECT_NO_BASE(
    Rectangle,
    field(x),
    field(y),
    field(width),
    field(height)
)