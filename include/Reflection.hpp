#pragma once

#include "CommonIncludes.hpp"

#include <vector>
#include <cassert>
#include <iostream>
#include <raylib.h>
#include "refl.hpp"
#include "PrimitiveTypes.hpp"

class TypeInfo;

struct FieldInfo {
    std::string name;
    const TypeInfo* type;

    void* (*GetPtr)(void*);
    const void* (*GetPtrConst)(const void*);
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

private:

    std::string m_name;
    std::vector<FieldInfo> m_fields;

    template<typename T>
    TypeInfo(refl::type_descriptor<T> td) : m_name(td.name) {
        refl::util::for_each(td.members, [this](auto member) {
            using Member = decltype(member);

            if constexpr (refl::descriptor::is_field(Member{})) {
                using FieldType = typename Member::value_type;

                FieldInfo info;

                info.name = member.name.c_str();
                info.type = &TypeInfo::Get<FieldType>();

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

