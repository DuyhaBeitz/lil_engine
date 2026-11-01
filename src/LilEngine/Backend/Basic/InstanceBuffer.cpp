#include "Render/InstanceBuffer.hpp"

#include <utility>
#include <vector>

#include <raymath.h>
#include <rlgl.h>

#include "Render/Pool.hpp"
#include "BackendAccess.hpp"

namespace Lil::Render {

    struct InstanceSlot {
        std::vector<InstanceInfo> instances;
        int count = 0;
    };

    static Pool<InstanceSlot> s_buffers;

    // ------------------------------------------------------------
    // Instancing shader
    //
    // raylib's DrawMeshInstanced() feeds the per-instance matrices to the
    // vertex attribute at shader.locs[SHADER_LOC_MATRIX_MODEL]. The default
    // material shader has no such attribute (the location is -1), so
    // drawing instanced meshes with it renders nothing and can corrupt the
    // mesh's own vertex attributes. Models loaded through LoadModel() use
    // the default shader, so a minimal instancing-capable shader is
    // substituted for them here.
    // ------------------------------------------------------------

    static const char* const kInstanceVertexShader = R"(#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in mat4 instanceTransform;

uniform mat4 mvp;

out vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp * instanceTransform * vec4(vertexPosition, 1.0);
}
)";

    static const char* const kInstanceFragmentShader = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
    vec4 texelColor = texture(texture0, fragTexCoord);
    finalColor = texelColor * colDiffuse * fragColor;
}
)";

    static Shader s_instanceShader{};
    static bool s_instanceShaderLoaded = false;
    static bool s_instanceShaderTried = false;

    // Returns nullptr when instancing is unavailable (unsupported GL
    // version, or the shader failed to compile/link).
    static const Shader* InstancingShader() {
        if (s_instanceShaderLoaded) {
            return &s_instanceShader;
        }

        if (s_instanceShaderTried || !IsWindowReady()) {
            return nullptr;
        }

        s_instanceShaderTried = true;

        const int glVersion = rlGetVersion();

        if (glVersion != RL_OPENGL_33 && glVersion != RL_OPENGL_43) {
            return nullptr;
        }

        Shader shader = LoadShaderFromMemory(
            kInstanceVertexShader,
            kInstanceFragmentShader
        );

        // A failed compile makes raylib hand back the default shader.
        if (shader.id == 0 || shader.id == rlGetShaderIdDefault()) {
            return nullptr;
        }

        const int instanceLoc =
            GetShaderLocationAttrib(shader, "instanceTransform");

        if (instanceLoc < 0) {
            UnloadShader(shader);
            return nullptr;
        }

        shader.locs[SHADER_LOC_MATRIX_MODEL] = instanceLoc;

        s_instanceShader = shader;
        s_instanceShaderLoaded = true;

        return &s_instanceShader;
    }

    // Called from CloseWindow(), while the GL context is still valid.
    void ReleaseInstanceShader() {
        if (s_instanceShaderLoaded) {
            UnloadShader(s_instanceShader);
        }

        s_instanceShader = Shader{};
        s_instanceShaderLoaded = false;
        s_instanceShaderTried = false;
    }

    static Matrix InstanceTransform(const InstanceInfo& instance) {
        const Matrix scale = MatrixScale(
            instance.scale.x,
            instance.scale.y,
            instance.scale.z
        );

        const Matrix rotation = QuaternionToMatrix(instance.rotation);

        const Matrix translation = MatrixTranslate(
            instance.position.x,
            instance.position.y,
            instance.position.z
        );

        // Match raylib's transform order: scale, rotate, translate.
        return MatrixMultiply(
            MatrixMultiply(scale, rotation),
            translation
        );
    }

    InstanceBuffer::InstanceBuffer() = default;

    InstanceBuffer::~InstanceBuffer() {
        Clear();
    }

    // Copying intentionally creates an empty buffer.
    InstanceBuffer::InstanceBuffer(const InstanceBuffer&) {}

    InstanceBuffer& InstanceBuffer::operator=(const InstanceBuffer&) {
        return *this;
    }

    InstanceBuffer::InstanceBuffer(InstanceBuffer&& other) noexcept
        : m_id(std::exchange(other.m_id, 0)) {}

    InstanceBuffer& InstanceBuffer::operator=(
        InstanceBuffer&& other
    ) noexcept {
        if (this != &other) {
            Clear();
            m_id = std::exchange(other.m_id, 0);
        }

        return *this;
    }

    void InstanceBuffer::Clear() {
        if (m_id == 0) {
            return;
        }

        if (s_buffers.Get(m_id)) {
            s_buffers.Free(m_id);
        }

        m_id = 0;
    }

    int InstanceBuffer::Count() const {
        const InstanceSlot* slot = s_buffers.Get(m_id);
        return slot ? slot->count : 0;
    }

    void InstanceBuffer::Upload(
        std::span<const InstanceInfo> infos
    ) {
        const int count = static_cast<int>(infos.size());

        if (count == 0) {
            Clear();
            return;
        }

        InstanceSlot* slot = s_buffers.Get(m_id);

        if (!slot) {
            m_id = s_buffers.Alloc(InstanceSlot{});

            if (m_id == 0) {
                return;
            }

            slot = s_buffers.Get(m_id);

            if (!slot) {
                m_id = 0;
                return;
            }
        }

        slot->instances.assign(infos.begin(), infos.end());
        slot->count = count;
    }

    void InstanceBuffer::Draw(const Assets::Model& model) const {
        const InstanceSlot* slot = s_buffers.Get(m_id);
        ::Model* raylibModel = Backend::GetRaylibModel(model);

        if (!slot ||
            !raylibModel ||
            slot->count <= 0 ||
            !raylibModel->meshes ||
            raylibModel->meshCount <= 0 ||
            !raylibModel->materials ||
            raylibModel->materialCount <= 0) {
            return;
        }

        std::vector<Matrix> transforms;
        transforms.reserve(slot->instances.size());

        for (const InstanceInfo& instance : slot->instances) {
            transforms.push_back(InstanceTransform(instance));
        }

        for (int meshIndex = 0;
             meshIndex < raylibModel->meshCount;
             ++meshIndex) {

            int materialIndex = 0;

            if (raylibModel->meshMaterial) {
                materialIndex = raylibModel->meshMaterial[meshIndex];
            }

            if (materialIndex < 0 ||
                materialIndex >= raylibModel->materialCount) {
                continue;
            }

            // Shallow copy: the maps array is shared, so the model's
            // textures and colors are still used. Only the shader differs.
            Material material = raylibModel->materials[materialIndex];

            const bool materialCanInstance =
                material.shader.locs &&
                material.shader.locs[SHADER_LOC_MATRIX_MODEL] != -1;

            if (!materialCanInstance) {
                if (const Shader* instancing = InstancingShader()) {
                    material.shader = *instancing;
                }
            }

            const bool canInstance =
                material.shader.locs &&
                material.shader.locs[SHADER_LOC_MATRIX_MODEL] != -1;

            if (canInstance) {
                DrawMeshInstanced(
                    raylibModel->meshes[meshIndex],
                    material,
                    transforms.data(),
                    static_cast<int>(transforms.size())
                );
            } else {
                // Instancing unavailable: draw each instance individually
                // so the model is still visible.
                for (const Matrix& transform : transforms) {
                    DrawMesh(
                        raylibModel->meshes[meshIndex],
                        material,
                        transform
                    );
                }
            }
        }
    }

} // namespace Lil::Render