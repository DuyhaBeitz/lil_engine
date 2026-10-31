#pragma once
#include <cstdint>
#include <span>
#include "Render/InstanceInfo.hpp"
#include "Assets/Model.hpp"

namespace Lil::Render {
    // Copying yields an empty buffer (re-uploaded on the next Upload()).
    class InstanceBuffer {
    public:
        
        InstanceBuffer();
        ~InstanceBuffer();
        InstanceBuffer(const InstanceBuffer&);
        InstanceBuffer& operator=(const InstanceBuffer&);
        InstanceBuffer(InstanceBuffer&&) noexcept;
        InstanceBuffer& operator=(InstanceBuffer&&) noexcept;

        void Upload(std::span<const InstanceInfo> infos);    // creates/grows, packs, uploads
        void Clear();                                        // frees GPU data
        int  Count() const;                                  // instances that will be drawn
        void Draw(const Assets::Model& model) const;
    private:
        uint32_t m_id = 0;
    };
}