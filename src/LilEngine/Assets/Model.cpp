#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"

#include <unordered_map>
#include <r3d/r3d.h>


#include "utils/NameHelper.hpp"
#include "utils/MathHelper.hpp"

namespace Lil::Assets {
    // manager

    static std::unordered_map<Model, R3D_Model> models;
    static std::unordered_map<Model, ModelSettings> model_settings;

    std::unordered_map<Model, ModelSettings> &ModelSettingsMap() {
        return model_settings;
    }

    void ModelDraw(Model handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale) {
        R3D_DrawModelEx(models.at(handle), pos, rot, scale);
    }
    RayCollision ModelRaycast(Model handle, const Ray& ray, const Transform& transform) {
        const Vector3& s = transform.scale;
        const Matrix matScale = MatrixScale(s.x, s.y, s.z);
        float angle;
        const Matrix matRotation = QuaternionToMatrix(transform.rotation);
        const Vector3& p = transform.translation;
        const Matrix matTranslation = MatrixTranslate(p.x, p.y, p.z);
        const Matrix mx = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
        return R3D_RaycastModel(ray, models.at(handle), mx);
    }
    BoundingBox ModelBounds(Model handle) { return models.at(handle).aabb; }

    static void MaterialSetAlbedo(R3D_Material& material_data, const AlbedoMap& map);
    static void MaterialSetNormal(R3D_Material& material_data, const NormalMap& map);
    static void MaterialSetEmission(R3D_Material& material_data, const EmissionMap& map);
    static void MaterialSetOrm(R3D_Material& material_data, const OrmMap& map);
    static AlbedoMap MaterialGetAlbedo(const R3D_Material& material_data);
    static NormalMap MaterialGetNormal(const R3D_Material& material_data);
    static EmissionMap MaterialGetEmission(const R3D_Material& material_data);
    static OrmMap MaterialGetOrm(const R3D_Material& material_data);

    static MaterialSettings GenMaterialSettings(const R3D_Material& material_data);
    static void MaterialSettingsApply(const MaterialSettings& settings, R3D_Material& material_data);
    static ModelSettings GenModelSettings(const R3D_Model& model_data);
    static void ModelSettingsApply(const ModelSettings& settings, R3D_Model& model_data);

    bool ModelExists(Model handle) {
        return models.find(handle) != models.end();
    }

    bool ModelSettingsExists(Model handle) {
        return model_settings.find(handle) != model_settings.end();
    }

    ModelSettings *GetModelSettings(Model handle)
    {
        if (!ModelSettingsExists(handle)) return nullptr;
        return &(model_settings.at(handle.Key()));
    }

    static void ModelAdd(Model handle, R3D_Model model_data) {
        if (ModelExists(handle)) R3D_UnloadModel(model_data, true);
        models[handle] = model_data;
        if (!ModelSettingsExists(handle)) model_settings[handle] = GenModelSettings(models.at(handle));
    }

    void ModelAdd(Model handle, std::string filename) {
        R3D_Model model_data = R3D_LoadModelEx(filename.c_str(), R3D_IMPORT_RETAIN_MESH_DATA);

        std::set<int> loaded_ids{};
        auto try_add_texture = [&loaded_ids](Texture handle, Texture2D& texture) {
            if (texture.id == 0 || loaded_ids.contains(texture.id)) return;
            std::string indx = std::to_string(loaded_ids.size());
            TextureAdd(std::string("Gen_") + indx + std::string("_") + handle.Key(), texture);
            loaded_ids.insert(texture.id);
        };

        for (int i = 0; i < model_data.materialCount; i++) {
            R3D_Material& mat = model_data.materials[i];
            std::string indx = std::to_string(i);
            try_add_texture(handle.Key(), mat.albedo.texture);
            try_add_texture(handle.Key(), mat.normal.texture);
            try_add_texture(handle.Key(), mat.emission.texture);
            try_add_texture(handle.Key(), mat.orm.texture);
        }
    }

    void ModelAdd(std::string filename) {
        Model handle = NameFromPath(filename);
        ModelAdd(handle, filename);
    }

    void ModelRemove(Model handle) {
        if (ModelExists(handle)) {
            R3D_UnloadModel(models.at(handle), true);
            models.erase(handle);
        }
    }
    void UpdateAllModelSettings() {
        for (auto& [handle, settings] : model_settings) {
            settings = GenModelSettings(models.at(handle));
        }
    }
    void ApplyAllModelSettings() {
        for (auto& [handle, settings] : model_settings) {
            ModelSettingsApply(settings, models.at(handle));
        }
    }





    void MaterialSetAlbedo(R3D_Material& material_data, const AlbedoMap& map) {
        material_data.albedo = {
            *GetTextureData(map.texture_key),
            map.color
        };
    }
    void MaterialSetNormal(R3D_Material& material_data, const NormalMap& map) {
        material_data.normal = {
            *GetTextureData(map.texture_key),
            map.scale
        };
    }
    void MaterialSetEmission(R3D_Material& material_data, const EmissionMap& map) {
        material_data.emission = {
            *GetTextureData(map.texture_key),
            WHITE,
            map.energy
        };
    }
    void MaterialSetOrm(R3D_Material& material_data, const OrmMap& map) {
        material_data.orm = {
            *GetTextureData(map.texture_key),
            map.occlusion,
            map.roughness,
            map.metalness,
            map.specular
        };
    }
    AlbedoMap MaterialGetAlbedo(const R3D_Material& material_data) {
        AlbedoMap map;
        map.texture_key = Lil::Assets::HandleFromTextureData(material_data.albedo.texture);
        map.color = material_data.albedo.color;
        return map;
    }
    NormalMap MaterialGetNormal(const R3D_Material& material_data) {
        NormalMap map;
        map.texture_key = Lil::Assets::HandleFromTextureData(material_data.normal.texture);
        map.scale = material_data.normal.scale;
        return map;
    }
    EmissionMap MaterialGetEmission(const R3D_Material& material_data) {
        EmissionMap map;
        map.texture_key = Lil::Assets::HandleFromTextureData(material_data.emission.texture);
        map.energy = material_data.emission.energy;
        return map;
    }
    OrmMap MaterialGetOrm(const R3D_Material& material_data) {
        OrmMap map;
        map.texture_key = Lil::Assets::HandleFromTextureData(material_data.orm.texture);
        map.occlusion = material_data.orm.occlusion;
        map.roughness = material_data.orm.roughness;
        map.metalness = material_data.orm.metalness;
        map.specular = material_data.orm.specular;
        return map;
    }
    MaterialSettings GenMaterialSettings(const R3D_Material& material_data) {
        MaterialSettings settings;
        settings.uvOffset = material_data.uvOffset;
        settings.uvScale  = material_data.uvScale;
        settings.albedo   = MaterialGetAlbedo(material_data);
        settings.normal   = MaterialGetNormal(material_data);
        settings.emission = MaterialGetEmission(material_data);
        settings.orm      = MaterialGetOrm(material_data);
        return settings;
    }
    void MaterialSettingsApply(const MaterialSettings& settings, R3D_Material& material_data) {
        material_data.uvScale = settings.uvScale;
        material_data.uvOffset = settings.uvOffset;
        MaterialSetAlbedo(material_data, settings.albedo);
        MaterialSetNormal(material_data, settings.normal);
        MaterialSetEmission(material_data, settings.emission);
        MaterialSetOrm(material_data, settings.orm);
    }
    ModelSettings GenModelSettings(const R3D_Model& model_data) {
        ModelSettings settings;
        for (int i = 0; i < model_data.materialCount; i++) {
            settings.material_settings.emplace_back(GenMaterialSettings(model_data.materials[i]));
        }
        return settings;
    }
    void ModelSettingsApply(const ModelSettings& settings, R3D_Model& model_data) {
        assert(model_data->materialCount == settings.size() && "materialCount and material_settings.size() don't match");
        for (int i = 0; i < model_data.materialCount; i++) {
            auto it = std::next(settings.material_settings.begin(), i);
            MaterialSettingsApply(*it, model_data.materials[i]);
        }
    }

    void UnloadAllModels() {
        for (auto& [handle, model] : models) {
            R3D_UnloadModel(model, true);
        }
        models.clear();
    }

} // namespace Assets