#pragma once

#include <raylib.h>
#include "AssetRef.hpp"
#include "Material.hpp"
#include "Reflection.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "utils/NameHelper.hpp"

struct ModelSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    std::list<MaterialSettings> material_settings;
};
LIL_REFLECT_EX(std::list<MaterialSettings>, bases<>, std_list_MaterialSettings)
LIL_REFLECT(ModelSettings, bases<>,
    field(material_settings, ContainerNoAddAttribute{}, ContainerNoEraseAttribute{})
)
LIL_SER_BEGIN(ModelSettings)
LIL_SER_FIELD(material_settings)
LIL_SER_END()

namespace Lil::Assets {
    // manager
    class Model : public AssetRef, public Reflectable {
    public:
        LIL_REFLECTABLE()
        using AssetRef::AssetRef;
    };
} // namespace Lil::Assets

LIL_REFLECT_EX(Lil::Assets::Model, bases<>, Lil_Assets_Model)
LIL_ASSET_REF_HASH(Lil::Assets::Model)

namespace Lil::Assets {
    std::unordered_map<Model, ModelSettings>& ModelSettingsMap();

    void ModelDraw(Model handle, const Vector3& pos, const Quaternion& rot, const Vector3& scale);
    RayCollision ModelRaycast(Model handle, const Ray& ray, const Transform& transform);
    BoundingBox ModelBounds(Model handle);

    ModelSettings* GetModelSettings(Model handle);
    bool ModelExists(Model handle);
    bool ModelSettingsExists(Model handle);
    void ModelAdd(Model handle, std::string filename);
    void ModelAdd(std::string filename);
    void ModelRemove(Model handle);

    void UpdateAllModelSettings();
    void ApplyAllModelSettings();

    template <class Archive>
    void save_models( Archive & ar ) {
        auto& model_settings = ModelSettingsMap();
        LIL_SER_FIELD(model_settings);
    }
        
    template <class Archive>
    void load_models( Archive & ar ) {
        auto& model_settings = ModelSettingsMap();
        LIL_SER_FIELD(model_settings);
        for (auto& [handle, settings] : ModelSettingsMap()) {
            if (IsAssetGenerated(handle.Key())) continue;
            ModelAdd(std::string("assets/") + handle.Key());
        }
        ApplyAllModelSettings();
    }

    void UnloadAllModels();
} // namespace Lil::Assets