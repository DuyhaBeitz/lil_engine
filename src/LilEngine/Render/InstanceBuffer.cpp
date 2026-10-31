#include "Render/InstanceBuffer.hpp"
#include <utility>
#include <r3d/r3d.h>
#include "Render/Pool.hpp"
#include "../AdvancedBackend/R3DAccess.hpp"

namespace Lil::Render {

    struct PackedRotation { int16_t x, y, z, w; };
    struct PackedScale    { uint16_t x, y, z; };

    static R3D_InstanceLayout instance_layout = {
        .formats = {
            R3D_INSTANCE_FORMAT_FLOAT32,    // position
            R3D_INSTANCE_FORMAT_SNORM16,    // rotation quaternion
            R3D_INSTANCE_FORMAT_FLOAT16,    // scale
            R3D_INSTANCE_FORMAT_UNORM8,     // color
        },
        .flags = R3D_INSTANCE_POSITION |
                 R3D_INSTANCE_ROTATION |
                 R3D_INSTANCE_SCALE |
                 R3D_INSTANCE_COLOR,
    };

    struct InstanceSlot { R3D_InstanceBuffer buffer; int capacity; int count; };
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
            std::cout << s << " " << &(s->buffer) << " " << s->capacity << " " << s->count << std::endl;

            
            R3D_UnloadInstanceBuffer(s->buffer);
            s_buffers.Free(m_id);
        }
        m_id = 0;
    }

    int InstanceBuffer::Count() const {
        InstanceSlot* s = s_buffers.Get(m_id);
        return s ? s->count : 0;
    }

    void InstanceBuffer::Upload(std::span<const InstanceInfo> infos) {
        const int n = int(infos.size());
        if (n == 0) { Clear(); return; }

        InstanceSlot* s = s_buffers.Get(m_id);
        if (!s) {
            m_id = s_buffers.Alloc(InstanceSlot{ R3D_LoadInstanceBufferEx(n, instance_layout), n, 0 });
            s = s_buffers.Get(m_id);
        } else if (n > s->capacity) {
            R3D_ResizeInstanceBuffer(&s->buffer, n, true);
            s->capacity = n;
        }
        // shrinking keeps capacity; Draw takes a count
        s->count = n;

        auto* positions = static_cast<Vector3*>       (R3D_MapInstances(s->buffer, R3D_INSTANCE_POSITION, false));
        auto* rotations = static_cast<PackedRotation*>(R3D_MapInstances(s->buffer, R3D_INSTANCE_ROTATION, false));
        auto* scales    = static_cast<PackedScale*>   (R3D_MapInstances(s->buffer, R3D_INSTANCE_SCALE, false));
        auto* colors    = static_cast<Color*>         (R3D_MapInstances(s->buffer, R3D_INSTANCE_COLOR, false));

        for (int i = 0; i < n; i++) {
            const InstanceInfo& in = infos[i];
            positions[i] = in.position;
            rotations[i] = PackedRotation{
                R3D_PackSnorm16(in.rotation.x), R3D_PackSnorm16(in.rotation.y),
                R3D_PackSnorm16(in.rotation.z), R3D_PackSnorm16(in.rotation.w)
            };
            scales[i] = PackedScale{
                R3D_PackFloat16(in.scale.x), R3D_PackFloat16(in.scale.y), R3D_PackFloat16(in.scale.z)
            };
            colors[i] = in.color;
        }

        R3D_UnmapInstances(s->buffer,
            R3D_INSTANCE_POSITION | R3D_INSTANCE_ROTATION |
            R3D_INSTANCE_SCALE    | R3D_INSTANCE_COLOR);
    }

    void InstanceBuffer::Draw(const Assets::Model& model) const {
        InstanceSlot* s = s_buffers.Get(m_id);
        R3D_Model* m = Backend::R3DModel(model);
        if (s && m && s->count > 0) R3D_DrawModelInstanced(*m, s->buffer, s->count);
    }
}