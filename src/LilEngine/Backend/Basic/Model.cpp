#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
#include <string>
#include <unordered_map>
#include "BackendAccess.hpp"

#include "utils/NameHelper.hpp"
#include "utils/MathHelper.hpp"

namespace Lil::Assets {

    // ------------------------------------------------------------
    // Model manager
    // ------------------------------------------------------------

    using RaylibModel = ::Model;

    static std::unordered_map<Model, RaylibModel> models;
    static std::unordered_map<Model, ModelSettings> model_settings;

    std::unordered_map<Model, ModelSettings>& ModelSettingsMap() {
        return model_settings;
    }

    // ------------------------------------------------------------
    // Model operations
    // ------------------------------------------------------------

    void ModelDraw(
        Model handle,
        const Vector3& pos,
        const Quaternion& rot,
        const Vector3& scale
    ) {
        auto it = models.find(handle);
        if (it == models.end()) return;

        // Convert the quaternion to the axis-angle representation
        // required by raylib's DrawModelEx().
        const float qx = rot.x;
        const float qy = rot.y;
        const float qz = rot.z;
        const float qw = rot.w;

        Vector3 axis = { 0.0f, 1.0f, 0.0f };
        float angle = 0.0f;

        const float qwClamped = std::clamp(qw, -1.0f, 1.0f);
        angle = 2.0f * std::acos(qwClamped);

        const float sinHalfAngle =
            std::sqrt(std::max(0.0f, 1.0f - qwClamped * qwClamped));

        if (sinHalfAngle > 0.00001f) {
            axis.x = qx / sinHalfAngle;
            axis.y = qy / sinHalfAngle;
            axis.z = qz / sinHalfAngle;
        }

        DrawModelEx(
            it->second,
            pos,
            axis,
            angle * RAD2DEG,
            scale,
            WHITE
        );
    }

    RayCollision ModelRaycast(
        Model handle,
        const Ray& ray,
        const Transform& transform
    ) {
        auto it = models.find(handle);

        if (it == models.end()) {
            return RayCollision{};
        }

        // Use a copy so the entity's transform does not permanently
        // modify the model stored in the asset manager.
        RaylibModel transformedModel = it->second;

        const Vector3& s = transform.scale;
        const Vector3& p = transform.translation;

        const Matrix matScale = MatrixScale(s.x, s.y, s.z);
        const Matrix matRotation = QuaternionToMatrix(transform.rotation);
        const Matrix matTranslation = MatrixTranslate(p.x, p.y, p.z);

        const Matrix transformMatrix = MatrixMultiply(
            MatrixMultiply(matScale, matRotation),
            matTranslation
        );

        transformedModel.transform = MatrixMultiply(
            transformedModel.transform,
            transformMatrix
        );

            
        RayCollision closest{};
        closest.distance = INFINITY;
        for (int i = 0; i < transformedModel.meshCount; ++i) {
            RayCollision hit = GetRayCollisionMesh(
                ray,
                transformedModel.meshes[i],
                transformedModel.transform
            );
            if (hit.hit && hit.distance < closest.distance) {
                closest = hit;
            }
        }
        if (!closest.hit) closest.distance = 0.0f;
        return closest;
    }

    BoundingBox ModelBounds(Model handle) {
        return GetModelBoundingBox(models.at(handle));
    }

    bool ModelExists(Model handle) {
        return models.find(handle) != models.end();
    }

    bool ModelSettingsExists(Model handle) {
        return model_settings.find(handle) != model_settings.end();
    }

    ModelSettings* GetModelSettings(Model handle) {
        auto it = model_settings.find(handle);

        if (it == model_settings.end()) {
            return nullptr;
        }

        return &it->second;
    }

    // ------------------------------------------------------------
    // Material map helpers
    // ------------------------------------------------------------

    static void MaterialSetAlbedo(
        Material& material_data,
        const AlbedoMap& map
    );

    static void MaterialSetNormal(
        Material& material_data,
        const NormalMap& map
    );

    static void MaterialSetEmission(
        Material& material_data,
        const EmissionMap& map
    );

    static void MaterialSetOrm(
        Material& material_data,
        const OrmMap& map
    );

    static AlbedoMap MaterialGetAlbedo(const Material& material_data);
    static NormalMap MaterialGetNormal(const Material& material_data);
    static EmissionMap MaterialGetEmission(const Material& material_data);
    static OrmMap MaterialGetOrm(const Material& material_data);

    static MaterialSettings GenMaterialSettings(
        const Material& material_data
    );

    static void MaterialSettingsApply(
        const MaterialSettings& settings,
        Material& material_data
    );

    static ModelSettings GenModelSettings(
        const RaylibModel& model_data
    );

    static void ModelSettingsApply(
        const ModelSettings& settings,
        RaylibModel& model_data
    );

    // ------------------------------------------------------------
    // Model creation and loading
    // ------------------------------------------------------------

    static void ModelAdd(Model handle, RaylibModel model_data) {
        auto existing = models.find(handle);
        const bool replaced = existing != models.end();

        if (replaced) {
            UnloadModel(existing->second);
            models.erase(existing);
        }

        models.emplace(handle, model_data);

        if (!ModelSettingsExists(handle)) {
            model_settings[handle] =
                GenModelSettings(models.at(handle));
        }

        // Only invalidate existing handles (e.g. animation players) when an
        // asset they may reference was actually replaced. Adding a brand new
        // model cannot affect anything that already exists; bumping here
        // made every AnimationPlayer go stale (and stop drawing) whenever
        // any unrelated model was loaded.
        if (replaced) {
            Backend::BumpAssetGeneration();
        }
    }

    // Translates a mesh that has already been uploaded to the GPU and
    // pushes the new positions to its vertex buffer. Normals and texcoords
    // are unaffected by a pure translation.
    static void MeshTranslate(Mesh& mesh, const Vector3& offset) {
        if (!mesh.vertices || mesh.vertexCount <= 0) {
            return;
        }

        if (offset.x == 0.0f && offset.y == 0.0f && offset.z == 0.0f) {
            return;
        }

        for (int i = 0; i < mesh.vertexCount; ++i) {
            mesh.vertices[i * 3 + 0] += offset.x;
            mesh.vertices[i * 3 + 1] += offset.y;
            mesh.vertices[i * 3 + 2] += offset.z;
        }

        // GenMeshHeightmap() already called UploadMesh(); buffer 0 holds
        // the vertex positions.
        UpdateMeshBuffer(
            mesh,
            0,
            mesh.vertices,
            mesh.vertexCount * 3 * static_cast<int>(sizeof(float)),
            0
        );
    }

    // raylib's GenMeshHeightmap() puts the terrain corner at the origin:
    //   x in [0, scale.x], z in [0, scale.z], y in [0, scale.y]
    // `offset` is added to every vertex so the visual mesh can be placed
    // where the physics heightfield is (Jolt: HeightFieldShapeSettings::
    // mOffset, i.e. sample (i, j) sits at offset + (i*sx, h, j*sz)).
    // The offset is baked into the vertices, so bounds, raycasts,
    // InstanceBuffer and AnimationPlayer all see the same geometry.
    void ModelAddHeightmap(
        Model handle,
        Image heightmap_image,
        const Vector3& scale,
        const Vector3& offset
    ) {
        Mesh mesh = GenMeshHeightmap(heightmap_image, scale);
        MeshTranslate(mesh, offset);

        RaylibModel model_data = LoadModelFromMesh(mesh);

        ModelAdd(handle, model_data);
    }

    // Unchanged behaviour: no offset (terrain corner at the origin).
    void ModelAddHeightmap(
        Model handle,
        Image heightmap_image,
        const Vector3& scale
    ) {
        ModelAddHeightmap(
            handle,
            heightmap_image,
            scale,
            scale * Vector3{ -0.5f, 0.0f, -0.5f }
        );
    }

    void ModelAdd(Model handle, std::string filename) {
        RaylibModel model_data = LoadModel(filename.c_str());

        if (model_data.meshCount <= 0) {
            // Do not register a model that failed to load.
            UnloadModel(model_data);
            return;
        }

        // Register imported material textures with the texture manager.
        // Avoid registering the same GPU texture more than once.
        std::set<unsigned int> loaded_ids;

        auto try_add_texture =
            [&loaded_ids, &handle](Texture2D& texture) {
                if (texture.id == 0 ||
                    loaded_ids.contains(texture.id)) {
                    return;
                }

                std::string index =
                    std::to_string(loaded_ids.size());

                TextureAdd(
                    std::string("Gen_") +
                    index +
                    std::string("_") +
                    handle.Key(),
                    texture
                );

                loaded_ids.insert(texture.id);
            };

        for (int i = 0; i < model_data.materialCount; ++i) {
            Material& material = model_data.materials[i];

            try_add_texture(
                material.maps[MATERIAL_MAP_ALBEDO].texture
            );

            try_add_texture(
                material.maps[MATERIAL_MAP_NORMAL].texture
            );

            try_add_texture(
                material.maps[MATERIAL_MAP_EMISSION].texture
            );

            try_add_texture(
                material.maps[MATERIAL_MAP_METALNESS].texture
            );

            try_add_texture(
                material.maps[MATERIAL_MAP_ROUGHNESS].texture
            );

            try_add_texture(
                material.maps[MATERIAL_MAP_OCCLUSION].texture
            );
        }

        ModelAdd(handle, model_data);
    }

    void ModelAdd(std::string filename) {
        Model handle = NameFromPath(filename);
        ModelAdd(handle, filename);
    }

    void ModelRemove(Model handle) {
        auto it = models.find(handle);

        if (it == models.end()) {
            return;
        }

        UnloadModel(it->second);
        models.erase(it);
        model_settings.erase(handle);

        Backend::BumpAssetGeneration();
    }

    // ------------------------------------------------------------
    // Model settings
    // ------------------------------------------------------------

    void UpdateAllModelSettings() {
        for (auto& [handle, settings] : model_settings) {
            auto it = models.find(handle);

            if (it == models.end()) {
                continue;
            }

            settings = GenModelSettings(it->second);
        }
    }

    void ApplyModelSettings(Model handle) {
        auto modelIt = models.find(handle);
        auto settingsIt = model_settings.find(handle);

        if (modelIt == models.end() ||
            settingsIt == model_settings.end()) {
            return;
        }

        ModelSettingsApply(settingsIt->second, modelIt->second);
    }

    void ApplyAllModelSettings() {
        for (auto& [handle, settings] : model_settings) {
            auto it = models.find(handle);

            if (it == models.end()) {
                continue;
            }

            ModelSettingsApply(settings, it->second);
        }
    }

    // ------------------------------------------------------------
    // Material map application
    // ------------------------------------------------------------

    void MaterialSetAlbedo(
        Material& material_data,
        const AlbedoMap& map
    ) {
        MaterialMap& albedo =
            material_data.maps[MATERIAL_MAP_ALBEDO];

        albedo.color = map.color;

        Texture2D* texture_data =
            GetTextureData(map.texture_key);

        if (texture_data) {
            albedo.texture = *texture_data;
        }
    }

    void MaterialSetNormal(
        Material& material_data,
        const NormalMap& map
    ) {
        MaterialMap& normal =
            material_data.maps[MATERIAL_MAP_NORMAL];

        normal.value = map.scale;

        Texture2D* texture_data =
            GetTextureData(map.texture_key);

        if (texture_data) {
            normal.texture = *texture_data;
        }
    }

    void MaterialSetEmission(
        Material& material_data,
        const EmissionMap& map
    ) {
        MaterialMap& emission =
            material_data.maps[MATERIAL_MAP_EMISSION];

        emission.color = WHITE;
        emission.value = map.energy;

        Texture2D* texture_data =
            GetTextureData(map.texture_key);

        if (texture_data) {
            emission.texture = *texture_data;
        }
    }

    void MaterialSetOrm(
        Material& material_data,
        const OrmMap& map
    ) {
        MaterialMap& metalness =
            material_data.maps[MATERIAL_MAP_METALNESS];

        MaterialMap& roughness =
            material_data.maps[MATERIAL_MAP_ROUGHNESS];

        MaterialMap& occlusion =
            material_data.maps[MATERIAL_MAP_OCCLUSION];

        metalness.value = map.metalness;
        roughness.value = map.roughness;
        occlusion.value = map.occlusion;

        // raylib stores metalness, roughness, and occlusion in separate
        // material maps. The Basic backend does not implement R3D's
        // packed ORM workflow, so the supplied texture is assigned to
        // each corresponding map.
        Texture2D* texture_data =
            GetTextureData(map.texture_key);

        if (texture_data) {
            metalness.texture = *texture_data;
            roughness.texture = *texture_data;
            occlusion.texture = *texture_data;
        }
    }

    // ------------------------------------------------------------
    // Material map extraction
    // ------------------------------------------------------------

    AlbedoMap MaterialGetAlbedo(
        const Material& material_data
    ) {
        AlbedoMap map;

        const MaterialMap& albedo =
            material_data.maps[MATERIAL_MAP_ALBEDO];

        map.texture_key =
            Lil::Assets::HandleFromTextureData(albedo.texture);

        map.color = albedo.color;

        return map;
    }

    NormalMap MaterialGetNormal(
        const Material& material_data
    ) {
        NormalMap map;

        const MaterialMap& normal =
            material_data.maps[MATERIAL_MAP_NORMAL];

        map.texture_key =
            Lil::Assets::HandleFromTextureData(normal.texture);

        map.scale = normal.value;

        return map;
    }

    EmissionMap MaterialGetEmission(
        const Material& material_data
    ) {
        EmissionMap map;

        const MaterialMap& emission =
            material_data.maps[MATERIAL_MAP_EMISSION];

        map.texture_key =
            Lil::Assets::HandleFromTextureData(emission.texture);

        map.energy = emission.value;

        return map;
    }

    OrmMap MaterialGetOrm(
        const Material& material_data
    ) {
        OrmMap map;

        const MaterialMap& metalness =
            material_data.maps[MATERIAL_MAP_METALNESS];

        const MaterialMap& roughness =
            material_data.maps[MATERIAL_MAP_ROUGHNESS];

        const MaterialMap& occlusion =
            material_data.maps[MATERIAL_MAP_OCCLUSION];

        // These maps are separate in raylib. Use the occlusion texture
        // as the representative texture for the combined ORM setting.
        map.texture_key =
            Lil::Assets::HandleFromTextureData(occlusion.texture);

        map.occlusion = occlusion.value;
        map.roughness = roughness.value;
        map.metalness = metalness.value;

        // raylib's standard MaterialMap does not expose R3D's separate
        // ORM specular property.
        map.specular = 0.0f;

        (void)metalness;

        return map;
    }

    // ------------------------------------------------------------
    // Material settings
    // ------------------------------------------------------------

    MaterialSettings GenMaterialSettings(
        const Material& material_data
    ) {
        MaterialSettings settings;

        // raylib's standard Material does not have R3D's UV offset and
        // scale fields. Keep these settings available through the
        // existing API, but do not apply unsupported UV transformations.
        settings.albedo = MaterialGetAlbedo(material_data);
        settings.normal = MaterialGetNormal(material_data);
        settings.emission = MaterialGetEmission(material_data);
        settings.orm = MaterialGetOrm(material_data);

        return settings;
    }

    void MaterialSettingsApply(
        const MaterialSettings& settings,
        Material& material_data
    ) {
        MaterialSetAlbedo(material_data, settings.albedo);
        MaterialSetNormal(material_data, settings.normal);
        MaterialSetEmission(material_data, settings.emission);
        MaterialSetOrm(material_data, settings.orm);

        // UV transforms are intentionally unsupported in the Basic
        // backend's standard raylib material implementation.
    }

    // ------------------------------------------------------------
    // Model settings
    // ------------------------------------------------------------

    ModelSettings GenModelSettings(
        const RaylibModel& model_data
    ) {
        ModelSettings settings;

        for (int i = 0; i < model_data.materialCount; ++i) {
            settings.material_settings.emplace_back(
                GenMaterialSettings(model_data.materials[i])
            );
        }

        return settings;
    }

    void ModelSettingsApply(
        const ModelSettings& settings,
        RaylibModel& model_data
    ) {
        assert(
            model_data.materialCount ==
                static_cast<int>(
                    settings.material_settings.size()
                ) &&
            "materialCount and material_settings.size() don't match"
        );

        for (int i = 0; i < model_data.materialCount; ++i) {
            const MaterialSettings* material =
                settings.Material(i);

            if (!material) {
                continue;
            }

            MaterialSettingsApply(
                *material,
                model_data.materials[i]
            );
        }
    }

    // ------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------

    void UnloadAllModels() {
        for (auto& [handle, model] : models) {
            (void)handle;
            UnloadModel(model);
        }

        models.clear();
        model_settings.clear();

        Backend::BumpAssetGeneration();
    }
} // namespace Lil::Assets


namespace Lil::Backend {
    ::Model* GetRaylibModel(const Assets::Model& handle) {
        auto it = Assets::models.find(handle);
        return it != Assets::models.end() ? &it->second : nullptr;
    }
} // namespace Lil::Backend