#pragma once

#include <raylib.h>
#include "AssetRef.hpp"
#include "Serialization.hpp"
#include "Reflection.hpp"
#include "utils/NameHelper.hpp"
#include "Texture.hpp"

struct AlbedoMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Lil::Assets::Texture texture_key;
    Color color = WHITE;
};
LIL_REFLECT(AlbedoMap, bases<>,
    field(texture_key),
    field(color)
)
LIL_SER_BEGIN(AlbedoMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(color)
LIL_SER_END()

struct NormalMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Lil::Assets::Texture texture_key;
    float scale = 1.0f;
};
LIL_REFLECT(NormalMap, bases<>,
    field(texture_key),
    field(scale)
)
LIL_SER_BEGIN(NormalMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(scale)
LIL_SER_END()

struct EmissionMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Lil::Assets::Texture texture_key;
    float energy = 1.0f;
};
LIL_REFLECT(EmissionMap, bases<>,
    field(texture_key),
    field(energy)
)
LIL_SER_BEGIN(EmissionMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(energy)
LIL_SER_END()

struct OrmMap : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Lil::Assets::Texture texture_key;
    float occlusion = 0.0f;
    float roughness = 1.0f;
    float metalness = 0.0f;
    float specular = 0.0f;
};
LIL_REFLECT(OrmMap, bases<>,
    field(texture_key),
    field(occlusion),
    field(roughness),
    field(metalness),
    field(specular)
)
LIL_SER_BEGIN(OrmMap)
LIL_SER_FIELD(texture_key)
LIL_SER_FIELD(occlusion)
LIL_SER_FIELD(roughness)
LIL_SER_FIELD(metalness)
LIL_SER_FIELD(specular)
LIL_SER_END()

struct MaterialSettings : Reflectable {
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    Vector2 uvScale = Vector2{1.0f, 1.0f};
    Vector2 uvOffset = Vector2{0.0f, 0.0f};
    AlbedoMap albedo;
    NormalMap normal;
    EmissionMap emission;
    OrmMap orm;
};
LIL_REFLECT(MaterialSettings, bases<>,
    field(uvScale),
    field(uvOffset),
    field(albedo),
    field(normal),
    field(emission),
    field(orm)
)
LIL_SER_BEGIN(MaterialSettings)
LIL_SER_FIELD(uvScale)
LIL_SER_FIELD(uvOffset)
LIL_SER_FIELD(albedo)
LIL_SER_FIELD(normal)
LIL_SER_FIELD(emission)
LIL_SER_FIELD(orm)
LIL_SER_END()
