#pragma once

#include "Actor.hpp"

class ModelComponent;

class Heightmap : public Actor {
private:
    void Setup(Image heightmap_image, Vector3 map_size);
    ModelComponent* model;

public:
    Heightmap(Image heightmap_image, Vector3 map_size);
    Heightmap(Texture2D heightmap_texture, Vector3 map_size);
    Heightmap(std::string texture_key, Vector3 map_size);

    virtual void CustomUpdate() override;
};