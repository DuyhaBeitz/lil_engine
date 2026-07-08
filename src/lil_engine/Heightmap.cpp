#include "Heightmap.hpp"
#include "utils/MeshHelper.hpp"
#include "utils/ColliderHelper.hpp"
#include "ResourceManager.hpp"
#include "Physics.hpp"

Heightmap::Heightmap(Image heightmap_image, Vector3 map_size) 
: Pawn(rc::BodyType::STATIC)
{
    ResourceManager::Get().AddModel("heightmap", HeightmapModel(heightmap_image, map_size));
    SetModel("heightmap", Vector3{-map_size.x/2, -map_size.y/2, -map_size.z/2});
    AddHeightmapCollider(heightmap_image, map_size, m_body);
}

Heightmap::Heightmap(Texture2D heightmap_texture, Vector3 map_size)
: Pawn(rc::BodyType::STATIC)
{
    Image heightmap_image = LoadImageFromTexture(heightmap_texture);

    ResourceManager::Get().AddModel("heightmap", HeightmapModel(heightmap_image, map_size));
    SetModel("heightmap", Vector3{-map_size.x/2, -map_size.y/2, -map_size.z/2});
    AddHeightmapCollider(heightmap_image, map_size, m_body);

    UnloadImage(heightmap_image);
}
