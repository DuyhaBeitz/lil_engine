#include "Render/Light.hpp"
#include <utility>

namespace Lil::Render {

    Light::Light() = default;

    Light::~Light() = default;

    Light::Light(const Light&) {}

    Light& Light::operator=(const Light&) {
        return *this;
    }

    Light::Light(Light&& other) noexcept
        : m_id(std::exchange(other.m_id, 0)) {}

    Light& Light::operator=(Light&& other) noexcept {
        if (this != &other) {
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

    void Light::Sync(const LightDesc&) {
    }

    void Light::DebugDraw() const {
    }

}
