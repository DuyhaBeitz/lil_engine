#include "Heightmap.hpp"
#include <LilEngine.hpp>
#include <utils/MeshHelper.hpp>
#include "Components/ColliderComponent.hpp"

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
