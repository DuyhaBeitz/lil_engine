#pragma once
#include <cstdint>
#include <deque>
#include <vector>

namespace Lil::Render {
    // id 0 = empty. Deque keeps element addresses stable across Alloc.
    template <class T>
    class Pool {
    public:
        uint32_t Alloc(const T& v) {
            if (!m_free.empty()) {
                uint32_t i = m_free.back(); m_free.pop_back();
                m_slots[i] = Slot{v, true};
                return i + 1;
            }
            m_slots.push_back(Slot{v, true});
            return uint32_t(m_slots.size());
        }
        T* Get(uint32_t id) {
            if (id == 0 || id > m_slots.size() || !m_slots[id - 1].used) return nullptr;
            return &m_slots[id - 1].value;
        }
        void Free(uint32_t id) {
            if (!Get(id)) return;
            m_slots[id - 1].used = false;
            m_free.push_back(id - 1);
        }
    private:
        struct Slot { T value; bool used; };
        std::deque<Slot> m_slots;
        std::vector<uint32_t> m_free;
    };
}