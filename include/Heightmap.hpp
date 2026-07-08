#pragma once

#include "Pawn.hpp"

class Heightmap : public Pawn {
public:
    Heightmap(Image heightmap_image, Vector3 map_size);
    Heightmap(Texture2D heightmap_texture, Vector3 map_size);
};