#include "Heightmap.hpp"
#include <LilEngine.hpp>
#include <utils/MeshHelper.hpp>
#include "Components/ColliderComponent.hpp"
#include "Components/ModelComponent.hpp"

void Heightmap::LayoutUpdate() {
    Actor::LayoutUpdate();
    
    std::string heightmap_name = HeightmapNameFromImageName(m_heightmap_texture_key);
    if (Lil::Resources().TextureExists(m_heightmap_texture_key)) {
        if (!Lil::Resources().ModelExists(heightmap_name)) {
            Image image = LoadImageFromTexture(*Lil::Resources().GetTexture(m_heightmap_texture_key));
            Lil::Resources().ModelAdd(heightmap_name, HeightmapModel(image, Vector3{1.0f, 1.0f, 1.0f}));
            Lil::Resources().GetModel(heightmap_name)->materials[0].albedo.texture = *Lil::Resources().GetTexture(m_heightmap_texture_key);
            UnloadImage(image);
        }

        if (m_model) {
            m_model->SetModel(heightmap_name);
            //m_model->Local().translation = GetScale() * Vector3{-0.5f, 0.0f, -0.5f};
        }
        if (m_collider && m_collider->m_shapes.size() > 0) {
            CollisionShape* shape = &m_collider->m_shapes.front();
            std::string old_key = shape->m_heightmap_texture_key;
            shape->m_heightmap_texture_key = m_heightmap_texture_key;
            if (old_key != shape->m_heightmap_texture_key) shape->m_needs_rebuild = true;

            Vector3 old_scale = shape->m_map_size;
            shape->m_map_size = GetScale();
            if (old_scale != shape->m_map_size) shape->m_needs_rebuild = true;
        }
    }
}

float Heightmap::GetHeightAt(float x, float z) {
    Texture2D* heightmap = Lil::Resources().GetTexture(m_heightmap_texture_key);

    if (!heightmap)return 0.0f;

    // World position -> normalized [0, 1] heightmap coordinates.
    float u = (x - GetPosition().x) / GetScale().x + 0.5f;
    float v = (z - GetPosition().z) / GetScale().z + 0.5f;

    if (u < 0.0f || u > 1.0f ||
        v < 0.0f || v > 1.0f)
        return 0.0f;

    // The mesh uses mapWidth/mapHeight vertices, with the
    // first vertex at 0 and the last at width-1/height-1.
    int px = static_cast<int>(u * (heightmap->width - 1));
    int pz = static_cast<int>(v * (heightmap->height - 1));

    Image img = LoadImageFromTexture(*heightmap);

    float height =
        static_cast<float>(GetImageColor(img, px, pz).r) / 255.0f;

    UnloadImage(img);

    // HeightmapModel multiplies the normalized height by size.y.
    return GetPosition().y + height * GetScale().y;
}

void Heightmap::RetrieveComponentPtrs() {
    m_model = GetFirst<ModelComponent>();
    m_collider = GetFirst<ColliderComponent>();
}

void Heightmap::SetupComponents() {
    m_model = Lil::World().CreateComponent<ModelComponent>();
    AttachComponent(m_model);
    m_model->MarkRequired();

    m_collider = Lil::World().CreateComponent<ColliderComponent>(BodyType::STATIC);
    AttachComponent(m_collider);
    m_collider->MarkRequired();

    CollisionShape shape;
    shape.m_type = CollisionShapeType::HEIGHTMAP;
    m_collider->AddShape(shape);
}
