#include "World.hpp"
#include "utils/ColliderHelper.hpp"
#include "lil_engine.hpp"

// std::shared_ptr<Actor> World::AddActor(std::shared_ptr<Actor> actor) {
//     m_actors[std::to_string(m_actors.size())] = actor;
//     return actor;
// }

// std::shared_ptr<Pawn> World::AddPawn(std::string model_key, Vector3 model_offset) {
//     auto pawn = std::make_shared<Pawn>();
//     pawn->SetModel(model_key, model_offset);
//     AddActor(pawn);
//     return pawn;
// }

// std::shared_ptr<Heightmap> World::AddHeightmap(Image heightmap_image, Vector3 map_size) {
//     auto heightmap = std::make_shared<Heightmap>(heightmap_image, map_size);
//     AddActor(heightmap);
//     return heightmap;
// }

// std::shared_ptr<Heightmap> World::AddHeightmap(Texture2D heightmap_texture, Vector3 map_size) {
//     auto heightmap = std::make_shared<Heightmap>(heightmap_texture, map_size);
//     AddActor(heightmap);
//     return heightmap;
// }

// std::shared_ptr<Heightmap> World::AddHeightmap(std::string texture_key, Vector3 map_size) {
//     auto heightmap = std::make_shared<Heightmap>(*Lil::Resources().GetTexture(texture_key), map_size);
//     AddActor(heightmap);
//     return heightmap;
// }

void World::DestroyActor(Actor *actor) {
    if (!actor) return;
    auto it = m_actors.find(actor);
    if (it != m_actors.end()) {
        m_actors.erase(it);
    }
}

bool World::IsActorAlive(Actor *actor) const {
    return actor && m_actors.find(actor) != m_actors.end();
}

void World::DestroyComponent(Component *component) {
    if (!component) return;
    auto it = m_components.find(component);
    if (it != m_components.end()) {
        m_components.erase(it);
    }
}

bool World::IsComponentAlive(Component *component) const {
    return component && m_components.find(component) != m_components.end();
}

void World::Draw() {
    for (auto& [key, actor] : m_actors) actor->Draw();
}

void World::Update() {
    float time_step = GetFrameTime()*m_simulation_speed;
    if (m_simulation_going && time_step > 0.0) Lil::Physics().GetWorld()->update(time_step);

    if (m_simulation_going) for (auto& [key, actor] : m_actors) actor->ComponentsUpdate();
    for (auto& [key, actor] : m_actors) actor->LayoutUpdate();

    // IF YOU WANT TO DRAW DEBUG WHILE PAUSING THE SIMULATION, SET TIMESTEP TO 0 (INSTEAD OF NOT CALLING UPDATE AT ALL)
    // WITHOUT UPDATING THE PHYSICS WORLD DEBUG DRAW IS VERY SLOW

    // ACTUALLY, PROVIDING 0 IS ILLEGAL HERE, SO
    // TODO: FIX PHYSICS DEBUG SLOW WHEN NO UPDATING
}

void World::DebugDraw(){
    for (auto& [key, actor] : m_actors) actor->DebugDraw();
    // rc::DebugRenderer& debugRenderer = Lil::Physics().GetWorld()->getDebugRenderer();
    // for (auto& [key, actor] : m_actors) {
    //     actor->GetBody()->setIsDebugEnabled(true);
    // }
    // Lil::Physics().GetWorld()->setIsDebugRenderingEnabled(true);
    // debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    // debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    // debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_POINT, false);
    // debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_NORMAL, false); 
    // debugRenderer.computeDebugRenderingPrimitives(*Lil::Physics().GetWorld());

    // rc::uint32 nbLines = debugRenderer.getNbLines();
    // for (rc::uint32 i = 0; i < nbLines; i++) {
    //     const rc::DebugRenderer::DebugLine& line = debugRenderer.getLines()[i];

    //     Color c = DARKBROWN;

    //     DrawLine3D(
    //         {line.point1.x, line.point1.y, line.point1.z},
    //         {line.point2.x, line.point2.y, line.point2.z},
    //         c
    //     );
    // }

    // nbLines = debugRenderer.getNbTriangles();
    // for (rc::uint32 i = 0; i < nbLines; i++) {
    //     const rc::DebugRenderer::DebugTriangle& triangle = debugRenderer.getTriangles()[i];

    //     Color c = Fade(RAYRED, 0.2);
    //     Vector3 points[3] = {
    //         {triangle.point1.x, triangle.point1.y+0.1f, triangle.point1.z},
    //         {triangle.point2.x, triangle.point2.y+0.1f, triangle.point2.z},
    //         {triangle.point3.x, triangle.point3.y+0.1f, triangle.point3.z}
    //     };

    //     Vector3 U = points[1]-points[0];
    //     Vector3 V = points[2]-points[0];
    //     Vector3 N = Vector3CrossProduct(U, V);
    //     N = Vector3Normalize(N);
    //     for (int i = 0; i < 3; i++) {
    //         points[i] = points[i] + N * 0.08f;
    //     }


    //     DrawTriangleStrip3D(&points[0], 3, c);
    // }
}
