#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"

#include <unordered_map>
#include <cfloat>
#include <lil3d/lil3d.h>
#include <rlgl.h>
#include "BackendAccess.hpp"
#include "MeshHelper.hpp"


#include "utils/NameHelper.hpp"
#include "utils/MathHelper.hpp"

namespace Lil::Assets {
    // manager

    static std::unordered_map<Model, LIL3D_Model> models;
    static std::unordered_map<Model, ModelSettings> model_settings;

    std::unordered_map<Model, ModelSettings> &ModelSettingsMap() {
        return model_settings;
    }

    void ModelDraw(Model handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale) {
        LIL3D_DrawModelPro(models.at(handle), Backend::ComposeTransform(pos, rot, scale));
    }
    RayCollision ModelRaycast(Model handle, const Ray& ray, const Transform& transform) {
        const LIL3D_Model& model = models.at(handle);
        const Matrix mx = Backend::ComposeTransform(transform.translation, transform.rotation, transform.scale);

        RayCollision best = {};
        best.distance = FLT_MAX;
        for (int i = 0; i < model.meshCount; i++) {
            RayCollision hit = GetRayCollisionMesh(ray, model.meshes[i].mesh, mx);
            if (hit.hit && hit.distance < best.distance) best = hit;
        }
        if (!best.hit) best = RayCollision{};
        return best;
    }
    BoundingBox ModelBounds(Model handle) { return models.at(handle).aabb; }

    // lil3d does not infer transparency from the asset. r3d treated materials as opaque unless the
    // asset asked otherwise, and raylib does not expose glTF alphaMode, so be conservative:
    //  - a texture whose alpha is clearly a cutout mask (a real share of fully transparent texels,
    //    and almost no semi-transparent ones, e.g. foliage) -> alpha test, double-sided
    //  - material colour alpha < 255                         -> alpha blending
    //  - anything else (including textures with stray / soft alpha, e.g. characters) -> opaque
    enum class AlphaKind { Opaque, Cutout, Blend };
    static std::unordered_map<unsigned int, AlphaKind> s_alpha_cache;   // by GL texture id

    static AlphaKind ScanTextureAlpha(const Texture2D& tex) {
        if (tex.id == 0 || tex.id == rlGetTextureIdDefault()) return AlphaKind::Opaque;
        switch (tex.format) {   // formats without an alpha channel
            case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
            case PIXELFORMAT_UNCOMPRESSED_R5G6B5:
            case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
            case PIXELFORMAT_UNCOMPRESSED_R32:
            case PIXELFORMAT_UNCOMPRESSED_R32G32B32:
            case PIXELFORMAT_UNCOMPRESSED_R16:
            case PIXELFORMAT_UNCOMPRESSED_R16G16B16:
                return AlphaKind::Opaque;
            default: break;
        }
        auto it = s_alpha_cache.find(tex.id);
        if (it != s_alpha_cache.end()) return it->second;

        AlphaKind kind = AlphaKind::Opaque;
        Image img = LoadImageFromTexture(tex);
        if (img.data) {
            ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            const unsigned char* p = static_cast<const unsigned char*>(img.data);
            const size_t n = size_t(img.width) * size_t(img.height);
            size_t clear = 0, soft = 0;
            for (size_t i = 0; i < n; i++) {
                unsigned char a = p[i * 4 + 3];
                if (a <= 5) clear++;
                else if (a < 250) soft++;
            }
            // mask-like: >= 5% fully transparent, and the semi-transparent texels are a small
            // minority of the non-opaque ones (anti-aliased edges only)
            if (clear * 100 >= n * 5 && soft * 4 <= clear) kind = AlphaKind::Cutout;
            UnloadImage(img);
        }
        s_alpha_cache[tex.id] = kind;
        return kind;
    }

    static void MaterialUpdateTransparency(LIL3D_Material& m) {
        AlphaKind kind = ScanTextureAlpha(m.albedo.texture);
        if (m.albedo.color.a < 255) kind = AlphaKind::Blend;

        m.alphaCutoff = 0.0f;
        m.blendMode   = LIL3D_BLEND_OPAQUE;
        m.cullMode    = LIL3D_CULL_BACK;
        if (kind == AlphaKind::Cutout)     { m.alphaCutoff = 0.5f; m.cullMode = LIL3D_CULL_NONE; }
        else if (kind == AlphaKind::Blend) { m.blendMode = LIL3D_BLEND_ALPHA; }
    }

    // Maps the backend has no equivalent for (normal map, specular, emission texture)
    // are accepted and ignored; they keep their defaults.
    static void MaterialSetAlbedo(LIL3D_Material& material_data, const AlbedoMap& map);
    static void MaterialSetNormal(LIL3D_Material& material_data, const NormalMap& map);
    static void MaterialSetEmission(LIL3D_Material& material_data, const EmissionMap& map);
    static void MaterialSetOrm(LIL3D_Material& material_data, const OrmMap& map);
    static AlbedoMap MaterialGetAlbedo(const LIL3D_Material& material_data);
    static NormalMap MaterialGetNormal(const LIL3D_Material& material_data);
    static EmissionMap MaterialGetEmission(const LIL3D_Material& material_data);
    static OrmMap MaterialGetOrm(const LIL3D_Material& material_data);

    static MaterialSettings GenMaterialSettings(const LIL3D_Material& material_data);
    static void MaterialSettingsApply(const MaterialSettings& settings, LIL3D_Material& material_data);
    static ModelSettings GenModelSettings(const LIL3D_Model& model_data);
    static void ModelSettingsApply(const ModelSettings& settings, LIL3D_Model& model_data);

    bool ModelExists(Model handle) {
        return models.find(handle) != models.end();
    }

    bool ModelSettingsExists(Model handle) {
        return model_settings.find(handle) != model_settings.end();
    }

    ModelSettings *GetModelSettings(Model handle) {
        if (!ModelSettingsExists(handle)) return nullptr;
        return &(model_settings.at(handle));
    }

    static void ModelAdd(Model handle, LIL3D_Model model_data) {
        // Replacing: free the OLD model's meshes. Its textures are also registered with the
        // texture manager (see ModelAdd(handle, filename)), which owns them, so keep them.
        if (auto it = models.find(handle); it != models.end()) LIL3D_UnloadModel(it->second, false);
        models[handle] = model_data;
        if (!ModelSettingsExists(handle)) model_settings[handle] = GenModelSettings(models.at(handle));
        Backend::BumpAssetGeneration();
    }

    void ModelAddHeightmap(Model handle, Image heightmap_image, const Vector3& scale) {
        ModelAdd(handle, HeightmapModel(heightmap_image, scale));
    }

    void ModelAdd(Model handle, std::string filename) {
        LIL3D_Model model_data = LIL3D_LoadModel(filename.c_str());

        std::set<int> loaded_ids{};
        auto try_add_texture = [&loaded_ids](Texture handle, Texture2D& texture) {
            if (texture.id == 0 || texture.id == rlGetTextureIdDefault() || loaded_ids.contains(texture.id)) return;
            std::string indx = std::to_string(loaded_ids.size());
            TextureAdd(std::string("Gen_") + indx + std::string("_") + handle.Key(), texture);
            loaded_ids.insert(texture.id);
        };

        for (int i = 0; i < model_data.materialCount; i++) {
            LIL3D_Material& mat = model_data.materials[i];
            try_add_texture(handle.Key(), mat.albedo.texture);
            try_add_texture(handle.Key(), mat.orm.texture);
        }

        for (int i = 0; i < model_data.materialCount; i++)
            MaterialUpdateTransparency(model_data.materials[i]);

        ModelAdd(handle, model_data);
    }

    void ModelAdd(std::string filename) {
        Model handle = NameFromPath(filename);
        ModelAdd(handle, filename);
    }

    void ModelRemove(Model handle) {
        if (ModelExists(handle)) {
            LIL3D_UnloadModel(models.at(handle), true);
            models.erase(handle);
            s_alpha_cache.clear();
            model_settings.erase(handle);
            Backend::BumpAssetGeneration();
        }
    }
    void UpdateAllModelSettings() {
        for (auto& [handle, settings] : model_settings) {
            auto it = models.find(handle);
            if (it == models.end()) continue;
            settings = GenModelSettings(it->second);
        }
    }
    void ApplyModelSettings(Model handle) {
        ModelSettingsApply(model_settings.at(handle), models.at(handle));
    }
    void ApplyAllModelSettings() {
        for (auto& [handle, settings] : model_settings) {
            auto it = models.find(handle);
            if (it == models.end()) continue;
            ModelSettingsApply(settings, it->second);
        }
    }





    void MaterialSetAlbedo(LIL3D_Material& material_data, const AlbedoMap& map) {
        material_data.albedo.color = map.color;

        if (Texture2D* texture_data = GetTextureData(map.texture_key))
            material_data.albedo.texture = *texture_data;
        MaterialUpdateTransparency(material_data);
    }
    void MaterialSetNormal(LIL3D_Material&, const NormalMap&) {}   // unsupported
    void MaterialSetEmission(LIL3D_Material& material_data, const EmissionMap& map) {
        // Only the energy is applied; the emission colour stays black (no emission
        // texture support, so a white colour would make the whole surface glow).
        material_data.emission.energy = map.energy;
    }
    void MaterialSetOrm(LIL3D_Material& material_data, const OrmMap& map) {
        material_data.orm.occlusion = map.occlusion;
        material_data.orm.roughness = map.roughness;
        material_data.orm.metalness = map.metalness;
        // map.specular: unsupported

        Texture2D* texture_data = GetTextureData(map.texture_key);
        if (!texture_data) return;
        material_data.orm.texture = *texture_data;
    }
    AlbedoMap MaterialGetAlbedo(const LIL3D_Material& material_data) {
        AlbedoMap map;
        map.texture_key = Lil::Assets::HandleFromTextureData(material_data.albedo.texture);
        map.color = material_data.albedo.color;
        return map;
    }
    NormalMap MaterialGetNormal(const LIL3D_Material&) {
        return NormalMap{};                                         // unsupported
    }
    EmissionMap MaterialGetEmission(const LIL3D_Material& material_data) {
        EmissionMap map;
        map.energy = material_data.emission.energy;                 // no emission texture
        return map;
    }
    OrmMap MaterialGetOrm(const LIL3D_Material& material_data) {
        OrmMap map;
        map.texture_key = Lil::Assets::HandleFromTextureData(material_data.orm.texture);
        map.occlusion = material_data.orm.occlusion;
        map.roughness = material_data.orm.roughness;
        map.metalness = material_data.orm.metalness;
        return map;                                                 // specular: unsupported
    }
    MaterialSettings GenMaterialSettings(const LIL3D_Material& material_data) {
        MaterialSettings settings;
        settings.uvOffset = material_data.uvOffset;
        settings.uvScale  = material_data.uvScale;
        settings.albedo   = MaterialGetAlbedo(material_data);
        settings.normal   = MaterialGetNormal(material_data);
        settings.emission = MaterialGetEmission(material_data);
        settings.orm      = MaterialGetOrm(material_data);
        return settings;
    }
    void MaterialSettingsApply(const MaterialSettings& settings, LIL3D_Material& material_data) {
        material_data.uvOffset = settings.uvOffset;
        material_data.uvScale  = settings.uvScale;
        MaterialSetAlbedo(material_data, settings.albedo);
        MaterialSetNormal(material_data, settings.normal);
        MaterialSetEmission(material_data, settings.emission);
        MaterialSetOrm(material_data, settings.orm);
    }
    ModelSettings GenModelSettings(const LIL3D_Model& model_data) {
        ModelSettings settings;
        for (int i = 0; i < model_data.materialCount; i++) {
            settings.material_settings.emplace_back(GenMaterialSettings(model_data.materials[i]));
        }
        return settings;
    }
    void ModelSettingsApply(const ModelSettings& settings, LIL3D_Model& model_data) {
        assert(model_data.materialCount == static_cast<int>(settings.material_settings.size()) && "materialCount and material_settings.size() don't match");
        for (int i = 0; i < model_data.materialCount; i++) {
            const MaterialSettings* mat = settings.Material(i);
            if (!mat) continue;
            MaterialSettingsApply(*mat, model_data.materials[i]);
        }
    }

    void UnloadAllModels() {
        for (auto& [handle, model] : models) {
            LIL3D_UnloadModel(model, true);
        }
        models.clear();
        s_alpha_cache.clear();
        Backend::BumpAssetGeneration();
    }

} // namespace Assets

namespace Lil::Backend {
    LIL3D_Model* GetRaylibModel(const Assets::Model& h) {
        auto it = Assets::models.find(h);
        return it == Assets::models.end() ? nullptr : &it->second;
    }
}