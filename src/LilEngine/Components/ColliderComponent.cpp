#include "Components/ColliderComponent.hpp"

#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "Physics.hpp"

#include "LilEngine.hpp"

ColliderComponent::ColliderComponent(rc::BodyType body_type)
: Component(), m_body(Lil::Physics().GetWorld()->createRigidBody(RcTransform(GetTransform())))
{
    m_body->setType(body_type);
}

rc::RigidBody *ColliderComponent::GetBody() { return m_body; }

void ColliderComponent::SetAngularLockAxisFactor(Vector3 lock_axis) {
    m_body->setAngularLockAxisFactor(rc::Vector3(lock_axis.x, lock_axis.y, lock_axis.z));
}

void ColliderComponent::SimulationUpdate(Actor &actor) {
    Transform t = GetTransform();
    t.translation = RlVector3(m_body->getTransform().getPosition());
    t.rotation = RlQuaternion(m_body->getTransform().getOrientation());
    SetTransform(t); // Sets this collider's world transform from physics

    Transform actorWorld = actor.GetTransform();

    actorWorld.rotation = QuaternionMultiply(
        m_transform.rotation,
        QuaternionInvert(m_local_transform.rotation)
    );

    Vector3 rotatedLocalPos = Vector3RotateByQuaternion(m_local_transform.translation, actorWorld.rotation);
    actorWorld.translation = m_transform.translation - m_local_transform.translation;
    actorWorld.scale = GetScale() / Local().scale;

    actor.SetTransform(actorWorld);
}

void ColliderComponent::DebugDraw() {
    rc::DebugRenderer& debugRenderer = Lil::Physics().GetWorld()->getDebugRenderer();
    debugRenderer.reset();
    GetBody()->setIsDebugEnabled(true);
    Lil::Physics().GetWorld()->setIsDebugRenderingEnabled(true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_POINT, false);
    debugRenderer.setIsDebugItemDisplayed(rc::DebugRenderer::DebugItem::CONTACT_NORMAL, false); 
    debugRenderer.computeDebugRenderingPrimitives(*Lil::Physics().GetWorld());
    

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