#include "Heightmap.hpp"
#include <LilEngine.hpp>
#include <utils/MeshHelper.hpp>
#include "Components/ColliderComponent.hpp"

void Heightmap::OnLayoutUpdate() {
    TryInitialize();

    std::string heightmap_name = HeightmapNameFromImageName(m_heightmap_texture_key);
    if (Lil::Resources().TextureExists(m_heightmap_texture_key)) {
        if (!Lil::Resources().ModelExists(heightmap_name)) {
            Image image = LoadImageFromTexture(*Lil::Resources().GetTexture(m_heightmap_texture_key));
            Lil::Resources().ModelAdd(heightmap_name, HeightmapModel(image, Vector3{1.0f, 1.0f, 1.0f}));
            UnloadImage(image);
        }
        else {
            if (m_model) {
                m_model->SetModel(heightmap_name);
                m_model->Local().translation = GetScale() * (-0.5f);
            }
            if (m_collider && m_shape) {
                m_shape->m_heightmap_texture_key = m_heightmap_texture_key;
                m_shape->m_map_size = GetScale();
            }
        }
    }
}

void Heightmap::TryInitialize() {
    if (m_model && m_collider) return;
    m_model = GetFirst<ModelComponent>();
    m_collider = GetFirst<ColliderComponent>();

    // components might be already loaded
    if (!m_model) {
        m_model = Lil::GetWorld().CreateComponent<ModelComponent>();
        AttachComponent(m_model);
    }

    if (!m_collider) {
        m_collider = Lil::GetWorld().CreateComponent<ColliderComponent>(BodyType::STATIC);
        CollisionShape shape;
        shape.m_type = CollisionShapeType::HEIGHTMAP;
        m_shape = m_collider->AddShape(shape);
        AttachComponent(m_collider);
    }
    else {
        m_shape = m_collider->GetFirstShape(CollisionShapeType::HEIGHTMAP);
    }
}
