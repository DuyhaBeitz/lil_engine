#include "Heightmap.hpp"
#include <LilEngine.hpp>
#include "Components/ColliderComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Assets.hpp"

void Heightmap::SetupComponents() {
    m_model = Lil::World().CreateComponent<ModelComponent>();
    AttachComponent(m_model.get());
    m_model->MarkRequired();

    m_collider = Lil::World().CreateComponent<ColliderComponent>(BodyType::STATIC);
    AttachComponent(m_collider.get());
    m_collider->MarkRequired();

    CollisionShape shape;
    shape.m_type = CollisionShapeType::HEIGHTMAP;
    m_collider->AddShape(shape);
}

void Heightmap::LayoutUpdate() {
    Actor::LayoutUpdate();
    
    std::string heightmap_name = HeightmapNameFromImageName(m_heightmap_texture_key.Key());
    if (Lil::Assets::TextureExists(m_heightmap_texture_key)) {
        Lil::Assets::Model handle(heightmap_name);
        if (!Lil::Assets::ModelExists(handle)) {
            bool settings_existed = Lil::Assets::ModelSettingsExists(handle);
            Image image = Lil::Assets::GetTextureImage(m_heightmap_texture_key);
            ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            TraceLog(LOG_INFO, "img: %dx%d fmt=%d data=%p", image.width, image.height, image.format, image.data);
            Lil::Assets::ModelAddHeightmap(handle, image, Vector3{1.0f, 1.0f, 1.0f});
            if (!settings_existed)
                Lil::Assets::GetModelSettings(handle)->Material(0)->albedo.texture_key = Lil::Assets::Texture(m_heightmap_texture_key);
            Lil::Assets::ApplyModelSettings(handle);
        }

        if (m_model) {
            m_model->SetModel(handle);
            //m_model->Local().translation = GetScale() * Vector3{-0.5f, 0.0f, -0.5f};
        }
        if (m_collider && m_collider->m_shapes.size() > 0) {
            CollisionShape* shape = &m_collider->m_shapes.front();
            Lil::Assets::Texture old_key = shape->m_heightmap_texture_key;
            shape->m_heightmap_texture_key = m_heightmap_texture_key;
            if (old_key != shape->m_heightmap_texture_key) shape->m_needs_rebuild = true;

            Vector3 old_scale = shape->m_map_size;
            shape->m_map_size = GetScale();
            if (old_scale != shape->m_map_size) shape->m_needs_rebuild = true;
        }
    }
}

HeightmapQueryResult Heightmap::QueryAt(float x, float z) {
    if (!Lil::Assets::TextureExists(m_heightmap_texture_key)) return {0};

    Image img = Lil::Assets::GetTextureImage(m_heightmap_texture_key);

    // World position -> normalized [0, 1] heightmap coordinates.
    float u = (x - GetPosition().x) / GetScale().x + 0.5f;
    float v = (z - GetPosition().z) / GetScale().z + 0.5f;

    if (u < 0.0f || u > 1.0f ||
        v < 0.0f || v > 1.0f)
        return {0};

    // The mesh uses mapWidth/mapHeight vertices, with the
    // first vertex at 0 and the last at width-1/height-1.
    int px = static_cast<int>(u * (img.width - 1));
    int pz = static_cast<int>(v * (img.height - 1));

    float value =
        static_cast<float>(GetImageColor(img, px, pz).r) / 255.0f;

    float height = GetPosition().y + value * GetScale().y;

    // HeightmapModel multiplies the normalized height by size.y.
    Ray ray = {
        .position = Vector3{x, height+0.5f, z},
        .direction = Vector3{0.0f,-1.0f,0.0f}
    };
    RayCollision collision = Raycast(ray);
    return HeightmapQueryResult{.value = value, .height = collision.point.y, .normal = collision.normal};
}