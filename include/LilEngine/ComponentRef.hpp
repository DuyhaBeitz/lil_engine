#pragma once

#include "stduuid/uuid.h"
#include "Serialization.hpp"

class Component;

Component* GetComponentByIdFromWorld(const uuids::uuid& id);
uuids::uuid ComponentIdOf(const Component* c);

template <class T>
class ComponentRef {
    Component* m_ptr = nullptr;

public:
    ComponentRef() = default;
    ComponentRef(T* p) : m_ptr(p) {}

    // These need complete T; they're only instantiated where called
    T* get() const        { return static_cast<T*>(m_ptr); }
    T* operator->() const { return get(); }

    explicit operator bool() const { return m_ptr != nullptr; }

    template <class Archive>
    std::string save_minimal(const Archive&) const {
        return m_ptr ? uuids::to_string(ComponentIdOf(m_ptr)) : std::string{};
    }

    template <class Archive>
    void load_minimal(const Archive&, const std::string& str) {
        m_ptr = nullptr;
        if (str.empty()) return;
        if (auto id = uuids::uuid::from_string(str)) m_ptr = GetComponentByIdFromWorld(*id);
    }
};