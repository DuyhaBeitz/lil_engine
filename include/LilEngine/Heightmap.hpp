#pragma once

#include "Actor.hpp"

class ModelComponent;
class ColliderComponent;

class Heightmap : public Actor {

public:
    // void Setup(Image heightmap_image, Vector3 map_size);
    LIL_REFLECTABLE()

    std::string m_heightmap_texture_key;

    ModelComponent* m_model = nullptr;
    ColliderComponent* m_collider = nullptr;

public:
    Heightmap() = default;

    virtual void SetupComponents() override;
    virtual void LayoutUpdate() override;

    template <class Archive>
    void save( Archive & ar ) const {
        LIL_SAVE_BASE(Actor)
        LIL_SER_FIELD(m_heightmap_texture_key);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        LIL_LOAD_BASE(Actor)
        LIL_SER_FIELD(m_heightmap_texture_key);
        m_model = GetFirst<ModelComponent>();
        m_collider = GetFirst<ColliderComponent>();
    }
};
LIL_REFLECT(Heightmap, bases<Actor>,
    field(m_heightmap_texture_key, TextureKeyAttribute())
)
LIL_DISAMBIGUATE_LOAD_SAVE(Heightmap)
LIL_SER_REGISTER_POLYMORPHIC(Heightmap)