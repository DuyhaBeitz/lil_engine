#include "Heightmap.hpp"
#include "CommonIncludes.hpp"
#include "utils/MeshHelper.hpp"
#include "utils/ColliderHelper.hpp"

#include "LilEngine.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/ColliderComponent.hpp"

void Heightmap::Setup(Image heightmap_image, Vector3 map_size) {
    Lil::Resources().AddModel("heightmap", HeightmapModel(heightmap_image, map_size));
    model = Lil::GetWorld().CreateComponent<ModelComponent>("heightmap");
    auto collider = Lil::GetWorld().CreateComponent<ColliderComponent>(rc::BodyType::STATIC);
    auto b = collider->GetBody();
    AttachComponent(model);
    Transform t1 = GetTransform();
    AttachComponent(collider);
    Transform t2 = GetTransform();

    model->SetModel("heightmap");
    model->Local().translation = Vector3{-map_size.x/2, -map_size.y/2, -map_size.z/2};
    AddHeightmapCollider(heightmap_image, map_size, b);
}

Heightmap::Heightmap() {
    model = Lil::GetWorld().CreateComponent<ModelComponent>("");
    auto collider = Lil::GetWorld().CreateComponent<ColliderComponent>(rc::BodyType::STATIC);
    auto b = collider->GetBody();
    AttachComponent(model);
    AttachComponent(collider);
}

Heightmap::Heightmap(Image heightmap_image, Vector3 map_size)
    : Actor()
{
    Setup(heightmap_image, map_size);
}

Heightmap::Heightmap(Texture2D heightmap_texture, Vector3 map_size)
    : Actor()
{
    Image heightmap_image = LoadImageFromTexture(heightmap_texture);
    Setup(heightmap_image, map_size);
    UnloadImage(heightmap_image);
}

Heightmap::Heightmap(std::string texture_key, Vector3 map_size) {
    Image heightmap_image = LoadImageFromTexture(*Lil::Resources().GetTexture(texture_key));
    Setup(heightmap_image, map_size);
    UnloadImage(heightmap_image);
}