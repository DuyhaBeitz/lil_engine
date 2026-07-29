#pragma once

#include "CommonIncludes.hpp"

#include <vector>
#include <set>
#include <list>
#include <cassert>
#include <iostream>
#include <raylib.h>
#include <unordered_map>
#include <functional>

#include "refl.hpp"

class TypeInfo;

struct FieldInfo {
    std::string parent_type_name;
    std::string name;
    const TypeInfo& type;
    std::set<std::string> attributes;

    void* (*GetPtr)(void*);
    const void* (*GetPtrConst)(const void*);

    bool HasAttribute(std::string attribute) const {
        return attributes.find(attribute) != attributes.end();
    }

    FieldInfo(const TypeInfo& type_) : type(type_) {}
};

class IVisitor {
public:
    virtual ~IVisitor() = default;

    virtual void VisitObject(const TypeInfo& ti, void* ptr) = 0;
    virtual void VisitObjectConst(const TypeInfo& ti, const void* ptr) = 0;
};

class ContainerInfo {
public:
    virtual size_t Size(const void* container) const = 0;
    virtual void* GetFirstElement(void* container) const = 0;
    virtual void* GetElement(void* container, size_t index) const = 0;
    virtual const void* GetElement(const void* container, size_t index) const = 0;
    virtual const TypeInfo& ElementType() const = 0;

    virtual void Resize(void* container, size_t size) const {}
    virtual void InsertDefault(void* container) const {}
    virtual void Erase(void* container, size_t index) const {}

    // fn takes idx and void element pointer as params
    virtual void ForEach(void* container, std::function<void(size_t, void*)> fn) const = 0;

    virtual ~ContainerInfo() = default;
};

template<typename T>
class VectorContainerInfo : public ContainerInfo {
public:
    size_t Size(const void* c) const override;
    void* GetFirstElement(void* container) const {return GetElement(container, 0);}
    void* GetElement(void* c, size_t i) const override;
    const void* GetElement(const void* container, size_t index) const override;
    const TypeInfo& ElementType() const;

    void Resize(void* c, size_t n) const override;
    void InsertDefault(void* c) const override;
    void Erase(void* c, size_t i) const override;

    void ForEach(void* container, std::function<void(size_t, void*)> fn) const override;
};

template<typename T>
class ListContainerInfo : public ContainerInfo {
public:
    size_t Size(const void* c) const override;
    void* GetFirstElement(void* container) const;
    void* GetElement(void* c, size_t i) const override;
    const void* GetElement(const void* container, size_t index) const override;
    const TypeInfo& ElementType() const override;

    void Resize(void* c, size_t n) const override;
    void InsertDefault(void* c) const override;
    void Erase(void* c, size_t i) const override;

    void ForEach(void* container, std::function<void(size_t, void*)> fn) const override;
};

template<typename T>
struct is_vector : std::false_type {};
template<typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};
template<typename T, typename A>
struct is_vector<const std::vector<T, A>> : std::true_type {};

template<typename T>
struct is_list : std::false_type {};
template<typename T, typename A>
struct is_list<std::list<T, A>> : std::true_type {};
template<typename T, typename A>
struct is_list<const std::list<T, A>> : std::true_type {};

template<typename T>
struct ValType {
    TypeInfo* Get() {return nullptr;}
};

template<typename T>
struct ValType<T*> {
    const TypeInfo* Get();
};

class TypeInfo {
public:

    template<typename T>
    static const TypeInfo& Get() {
        static const TypeInfo info(refl::reflect<std::remove_cvref_t<T>>());
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

    void* Create() const { if(m_create_fn) return m_create_fn(); else return nullptr; }

    template<typename T>
    bool IsA() const {
        return IsA_Internal<T>(this);
    }

    bool IsContainer() const {
        return m_container != nullptr;
    }

    const ContainerInfo* Container() const {
        return m_container.get();
    }

    bool operator==(const TypeInfo& other) const {
        return m_name == other.Name();
    }

    bool operator!=(const TypeInfo& other) const {
        return !(*this == other);
    }

    bool IsConst() const { return m_is_const; }
    bool IsPointer() const { return m_is_pointer; }
    const TypeInfo& ValueType() const {
        return *val_type;
    }

private:

    std::string m_name;
    std::vector<FieldInfo> m_fields;
    std::vector<const TypeInfo*> m_bases;
    bool m_is_const;
    std::function<void* ()> m_create_fn = nullptr;
    std::unique_ptr<ContainerInfo> m_container = nullptr;

    bool m_is_pointer;
    const TypeInfo* val_type;


    template<typename T>
    TypeInfo(refl::type_descriptor<T> td) : m_name(td.name), val_type(ValType<T>().Get()) {
        m_is_const = std::is_const_v<T>;
        m_is_pointer = std::is_pointer_v<T>;

        if constexpr (std::default_initializable<T>) {
            m_create_fn = [] { return static_cast<void*>(new T()); };
        }
        
        if constexpr (is_vector<T>::value) {
            using Elem = typename T::value_type;
            m_container = std::make_unique<VectorContainerInfo<Elem>>();
        }
        else if constexpr (is_list<T>::value) {
            using Elem = typename T::value_type;
            m_container = std::make_unique<ListContainerInfo<Elem>>();
        }
        else {
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

                    if constexpr (!std::is_const_v<T> && !std::is_const_v<FieldType>) {
                        info.GetPtr = [](void* object) -> void* {
                            auto* obj = static_cast<T*>(object);
                            return &(obj->*Member::pointer);
                        };
                    } else {
                        info.GetPtr = [](void*) -> void* {return nullptr;};
                    }

                    info.GetPtrConst = [](const void* object) -> const void* {
                        auto* obj = static_cast<const T*>(object);
                        return &(obj->*Member::pointer);
                    };

                    refl::util::for_each(refl::descriptor::get_attribute_types(member), [&info](auto t) {
                            using AttributeType = decltype(t);
                            const TypeInfo& ti = TypeInfo::Get<AttributeType>();
                            info.attributes.insert(ti.Name());
                        });

                    m_fields.push_back(std::move(info));
                }
            });
        }
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
            const auto& ti = TypeInfo::Get<T>();
            m_types[ti.Name()] = &ti;
        }

        std::unordered_map<std::string, const TypeInfo*>& Types() { return m_types; }
               
    private:
        std::unordered_map<std::string, const TypeInfo*> m_types{};
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

#define LIL_REFLECTABLE() \
    virtual const TypeInfo& GetTypeInfo() const override \
    { \
        return TypeInfo::Get<::refl::trait::remove_qualifiers_t<decltype(*this)>>(); \
    }

#define LIL_REFLECT_EX(typename, bases, registername, ...) \
    REFL_AUTO(type(typename, bases) __VA_OPT__(,) __VA_ARGS__) \
    static TypeRegisterer<typename> _##registername##_registerer;

#define LIL_REFLECT(typename, bases, ...) \
    LIL_REFLECT_EX(typename, bases, typename, __VA_ARGS__)

LIL_REFLECT(
    Vector2,
    bases<>,
    field(x),
    field(y)
)

LIL_REFLECT(
    Vector3,
    bases<>,
    field(x),
    field(y),
    field(z)
)

LIL_REFLECT(
    Color,
    bases<>,
    field(r),
    field(g),
    field(b),
    field(a)
)

LIL_REFLECT(
    Vector4,
    bases<>,
    field(x),
    field(y),
    field(z),
    field(w)
)

LIL_REFLECT(
    Transform,
    bases<>,
    field(translation),
    field(rotation),
    field(scale)
)

LIL_REFLECT(
    Rectangle,
    bases<>,
    field(x),
    field(y),
    field(width),
    field(height)
)

template <typename T>
inline size_t VectorContainerInfo<T>::Size(const void *c) const {
    return static_cast<const std::vector<T>*>(c)->size();
}

template <typename T>
inline void *VectorContainerInfo<T>::GetElement(void *c, size_t i) const {
    return &(*static_cast<std::vector<T>*>(c)).at(i);
}

template <typename T>
inline const void *VectorContainerInfo<T>::GetElement(const void *c, size_t i) const {
    return &(*static_cast<const std::vector<T>*>(c)).at(i);
}

template <typename T>
inline const TypeInfo &VectorContainerInfo<T>::ElementType() const {
    return TypeInfo::Get<T>();
}

template <typename T>
inline void VectorContainerInfo<T>::Resize(void *c, size_t n) const {
    static_cast<std::vector<T>*>(c)->resize(n);
}

template <typename T>
inline void VectorContainerInfo<T>::InsertDefault(void *c) const {
    static_cast<std::vector<T>*>(c)->emplace_back();
}

template <typename T>
inline void VectorContainerInfo<T>::Erase(void *c, size_t i) const {
    auto& v = *static_cast<std::vector<T>*>(c);
    v.erase(v.begin() + i);
}

template <typename T>
inline void VectorContainerInfo<T>::ForEach(void *c, std::function<void(size_t, void*)> fn) const {
    auto& v = *static_cast<std::vector<T>*>(c);
    for (size_t i = 0; i < v.size(); i++) {
        fn(i, &v.at(i));
    };
}

template <typename T>
inline const TypeInfo *ValType<T *>::Get() {
    return &TypeInfo::Get<T>();
}

template <typename T>
inline size_t ListContainerInfo<T>::Size(const void *c) const {
    return static_cast<const std::list<T>*>(c)->size();
}

template <typename T>
inline void *ListContainerInfo<T>::GetFirstElement(void *c) const {
    return &(static_cast<std::list<T>*>(c)->front());
}

template <typename T>
inline void *ListContainerInfo<T>::GetElement(void *c, size_t i) const {
    if (!c) return nullptr;
    auto& list = *static_cast<std::list<T>*>(c);
    if (i >= list.size()) return nullptr;
    auto it = std::next(list.begin(), i);
    return std::addressof(*it); 
}

template <typename T>
inline const void *ListContainerInfo<T>::GetElement(const void *c, size_t i) const {
    if (!c) return nullptr;
    auto& list = *static_cast<const std::list<T>*>(c);
    if (i >= list.size()) return nullptr;
    auto it = std::next(list.begin(), i);
    return std::addressof(*it); 
}

template <typename T>
inline const TypeInfo &ListContainerInfo<T>::ElementType() const {
    return TypeInfo::Get<T>();
}

template <typename T>
inline void ListContainerInfo<T>::Resize(void *c, size_t n) const
{
    static_cast<std::list<T>*>(c)->resize(n);
}

template <typename T>
inline void ListContainerInfo<T>::InsertDefault(void *c) const {
    static_cast<std::list<T>*>(c)->emplace_back();
}

template <typename T>
inline void ListContainerInfo<T>::Erase(void *c, size_t i) const {
    if (!c) return;
    auto& list = *static_cast<std::list<T>*>(c);
    if (i >= list.size()) return;
    auto it = std::next(list.begin(), i);
    list.erase(it);
}

template <typename T>
inline void ListContainerInfo<T>::ForEach(void *c, std::function<void(size_t, void *)> fn) const {
    auto& l = *static_cast<std::list<T>*>(c);
    size_t i = 0;
    for (auto& element : l) {
        fn(i, static_cast<void*>(&element));
        i++;
    }
}
