#include "World.hpp"
#include "utils/ColliderHelper.hpp"

std::shared_ptr<Actor> World::AddActor(std::shared_ptr<Actor> actor) {
    m_actors[std::to_string(m_actors.size())] = actor;
    return actor;
}

void World::Draw() {
    for (auto& [key, actor] : m_actors) actor->Draw();
}

void World::Update() {
    float time_step = GetFrameTime()*m_simulation_speed;
    if (m_simulation_going && time_step > 0.0) Physics::Get().GetWorld()->update(time_step);

    // IF YOU WANT TO DRAW DEBUG WHILE PAUSING THE SIMULATION, SET TIMESTEP TO 0 (INSTEAD OF NOT CALLING UPDATE AT ALL)
    // WITHOUT UPDATING THE PHYSICS WORLD DEBUG DRAW IS VERY SLOW

    // ACTUALLY, PROVIDING 0 IS ILLEGAL HERE, SO
    // TODO: FIX PHYSICS DEBUG SLOW WHEN NO UPDATING
}

void World::DebugDraw(){
    rc::DebugRenderer& debugRenderer = Physics::Get().GetWorld()->getDebugRenderer();
    for (auto& [key, actor] : m_actors) {
        actor->GetBody()->setIsDebugEnabled(true);
    }
    Physics::Get().GetWorld()->setIsDebugRenderingEnabled(true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_POINT, false);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_NORMAL, false); 
    debugRenderer.computeDebugRenderingPrimitives(*Physics::Get().GetWorld());

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
