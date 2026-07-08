#include "World.hpp"

#include "utils/ColliderHelper.hpp"

std::shared_ptr<Actor> World::AddActor(rc::BodyType bodyType) {
    auto actor = std::make_shared<Actor>(m_world->createRigidBody(rc::Transform{}));
    actor->GetBody()->setType(bodyType);
    m_actors[std::to_string(m_actors.size())] = actor;
    return actor;
}

std::shared_ptr<Actor> World::AddHeightmapActor(Image heightmap_image, Vector3 map_size)
{
    ResourceManager::Get().AddModel("heightmap", HeightmapModel(heightmap_image, map_size));

    auto map = AddActor(rc::BodyType::STATIC);
    map->SetModel("heightmap", Vector3{-map_size.x/2, -map_size.y/2, -map_size.z/2});
    AddHeightmapCollider(heightmap_image, map_size, m_physicsCommon, map->GetBody());
    map->SetPosition({0, 0, 0});
    auto map_body = map->GetBody();

    UnloadImage(heightmap_image);
    return map;
}

std::shared_ptr<Actor> World::AddHeightmapActor(Texture2D heightmap_texture, Vector3 map_size) {
    Image heightmap_image = LoadImageFromTexture(heightmap_texture);
    return AddHeightmapActor(heightmap_image, map_size);
}

void World::Draw() {
    for (auto& [key, actor] : m_actors) actor->Draw();
}

void World::Update()
{
    float time_step = GetFrameTime()*m_simulation_speed;
    if (m_simulation_going && time_step > 0.0) m_world->update(time_step);

    // IF YOU WANT TO DRAW DEBUG WHILE PAUSING THE SIMULATION, SET TIMESTEP TO 0 (INSTEAD OF NOT CALLING UPDATE AT ALL)
    // WITHOUT UPDATING THE PHYSICS WORLD DEBUG DRAW IS VERY SLOW

    // ACTUALLY, PROVIDING 0 IS ILLEGAL HERE, SO
    // TODO: FIX PHYSICS DEBUG SLOW WHEN NO UPDATING
}

void World::DebugDraw()
{
    rc::DebugRenderer& debugRenderer = m_world->getDebugRenderer();
    for (auto& [key, actor] : m_actors) {
        actor->GetBody()->setIsDebugEnabled(true);
    }
    m_world->setIsDebugRenderingEnabled(true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_POINT, false);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_NORMAL, false); 
    debugRenderer.computeDebugRenderingPrimitives(*m_world);

    rc::uint32 nbLines = debugRenderer.getNbLines();
    for (rc::uint32 i = 0; i < nbLines; i++) {
        const rc::DebugRenderer::DebugLine& line = debugRenderer.getLines()[i];

        Color c = DARKBROWN;

        DrawLine3D(
            {line.point1.x, line.point1.y, line.point1.z},
            {line.point2.x, line.point2.y, line.point2.z},
            c
        );
    }

    nbLines = debugRenderer.getNbTriangles();
    for (rc::uint32 i = 0; i < nbLines; i++) {
        const rc::DebugRenderer::DebugTriangle& triangle = debugRenderer.getTriangles()[i];

        Color c = Fade(RAYRED, 0.2);
        Vector3 points[3] = {
            {triangle.point1.x, triangle.point1.y+0.1f, triangle.point1.z},
            {triangle.point2.x, triangle.point2.y+0.1f, triangle.point2.z},
            {triangle.point3.x, triangle.point3.y+0.1f, triangle.point3.z}
        };

        Vector3 U = points[1]-points[0];
        Vector3 V = points[2]-points[0];
        Vector3 N = Vector3CrossProduct(U, V);
        N = Vector3Normalize(N);
        for (int i = 0; i < 3; i++) {
            points[i] = points[i] + N * 0.08f;
        }


        DrawTriangleStrip3D(&points[0], 3, c);
    }
}
