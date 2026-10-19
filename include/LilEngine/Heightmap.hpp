#pragma once

#include "Actor.hpp"
#include "ReflAttributes.hpp"
#include "ComponentRef.hpp"

class ModelComponent;
class ColliderComponent;

struct HeightmapQueryResult {
    float value;   // [0; 1]
    float height;  // world y coordinate
    Vector3 normal;
};

class Heightmap : public Actor {
public:
    // void Setup(Image heightmap_image, Vector3 map_size);
    LIL_REFLECTABLE()

    std::string m_heightmap_texture_key;

    ComponentRef<ModelComponent> m_model;
    ComponentRef<ColliderComponent> m_collider;

public:
    Heightmap() = default;
    virtual ~Heightmap() = default;

    virtual void SetupComponents() override;
    virtual void LayoutUpdate() override;

    HeightmapQueryResult QueryAt(float x, float z);

    template <class Archive>
    void save( Archive & ar ) const {
        LIL_SAVE_BASE(Actor)
        LIL_SER_FIELD(m_heightmap_texture_key);
        LIL_SER_FIELD(m_model);
        LIL_SER_FIELD(m_collider);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        LIL_LOAD_BASE(Actor)
        LIL_SER_FIELD(m_heightmap_texture_key);
        LIL_SER_FIELD(m_model);
        LIL_SER_FIELD(m_collider);
    }
};
LIL_REFLECT(Heightmap, bases<Actor>,
    field(m_heightmap_texture_key, TextureKeyAttribute())
)
LIL_DISAMBIGUATE_LOAD_SAVE(Heightmap)
LIL_SER_REGISTER_POLYMORPHIC(Heightmap)