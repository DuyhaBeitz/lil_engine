#include "Render/InstanceBuffer.hpp"
#include <utility>
#include <lil3d/lil3d.h>
#include "Render/Pool.hpp"
#include "BackendAccess.hpp"

namespace Lil::Render {

    // Per-instance colour is not supported by the backend and is ignored.
    struct InstanceSlot { LIL3D_InstanceBuffer buffer; int count; };
    static Pool<InstanceSlot> s_buffers;

    InstanceBuffer::InstanceBuffer() = default;
    InstanceBuffer::~InstanceBuffer() { Clear(); }
    InstanceBuffer::InstanceBuffer(const InstanceBuffer&) {}
    InstanceBuffer& InstanceBuffer::operator=(const InstanceBuffer&) { return *this; }
    InstanceBuffer::InstanceBuffer(InstanceBuffer&& o) noexcept : m_id(std::exchange(o.m_id, 0)) {}
    InstanceBuffer& InstanceBuffer::operator=(InstanceBuffer&& o) noexcept {
        if (this != &o) { Clear(); m_id = std::exchange(o.m_id, 0); }
        return *this;
    }

    void InstanceBuffer::Clear() {
        if (m_id == 0) return;
        if (InstanceSlot* s = s_buffers.Get(m_id)) {
            LIL3D_UnloadInstanceBuffer(s->buffer);
            s_buffers.Free(m_id);
        }
        m_id = 0;
    }

    int InstanceBuffer::Count() const {
        InstanceSlot* s = s_buffers.Get(m_id);
        return s ? s->count : 0;
    }

    // NOTE: the backend keeps a pointer to the buffer until the scene is ended,
    // so Upload()/Clear() must not be called between Draw() and EndScene().
    void InstanceBuffer::Upload(std::span<const InstanceInfo> infos) {
        const int n = int(infos.size());
        if (n == 0) { Clear(); return; }

        InstanceSlot* s = s_buffers.Get(m_id);
        if (!s) {
            m_id = s_buffers.Alloc(InstanceSlot{ LIL3D_LoadInstanceBuffer(n), 0 });
            s = s_buffers.Get(m_id);
        } else if (n > s->buffer.capacity) {
            LIL3D_UnloadInstanceBuffer(s->buffer);
            s->buffer = LIL3D_LoadInstanceBuffer(n);
        }
        // shrinking keeps capacity; Draw takes a count
        Matrix* transforms = LIL3D_MapInstances(s->buffer);
        if (!transforms) { s->count = 0; return; }
        s->count = n;

        for (int i = 0; i < n; i++) {
            const InstanceInfo& in = infos[i];
            transforms[i] = Backend::ComposeTransform(in.position, in.rotation, in.scale);
        }
    }

    void InstanceBuffer::Draw(const Assets::Model& model) const {
        InstanceSlot* s = s_buffers.Get(m_id);
        LIL3D_Model* m = Backend::GetRaylibModel(model);
        if (s && m && s->count > 0) LIL3D_DrawModelInstanced(*m, s->buffer, s->count);
    }
}
