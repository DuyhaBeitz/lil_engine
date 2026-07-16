#pragma once

#include "Actor.hpp"

class ModelComponent;

class Heightmap : public Actor {
public:
    void Setup(Image heightmap_image, Vector3 map_size);
    ModelComponent* model;

public:
    LIL_REFLECTABLE()

    Heightmap(Image heightmap_image, Vector3 map_size);
    Heightmap(Texture2D heightmap_texture, Vector3 map_size);
    Heightmap(std::string texture_key, Vector3 map_size);
};
REFL_AUTO(type(Heightmap, bases<Actor>))