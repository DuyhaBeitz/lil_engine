#include "Components/ColliderComponent.hpp"

#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "Physics.hpp"

#include "LilEngine.hpp"
#include <utils/ColliderHelper.hpp>

ColliderComponent::ColliderComponent(BodyType body_type)
: Component(), m_body(Lil::Physics().GetWorld()->createRigidBody(RcTransform(GetTransform())))
{
    m_body->setType(GetBodyType(body_type));
}

ColliderComponent::~ColliderComponent() {
    m_shapes.clear();
    if (m_body) Lil::Physics().GetWorld()->destroyRigidBody(m_body);
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

    m_linear_velocity = RlVector3(m_body->getLinearVelocity());
    m_angular_velocity = RlVector3(m_body->getAngularVelocity());

}

void ColliderComponent::DebugUpdate() {
    GetBody()->setIsDebugEnabled(true);
}
void ColliderComponent::DebugDraw() {
    rc::DebugRenderer& debugRenderer = Lil::Physics().GetWorld()->getDebugRenderer();   

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
        for (int j = 0; j < 3; j++) {
            points[j] = points[j] + N * 0.08f;
        }


        DrawTriangleStrip3D(&points[0], 3, c);
    }
}

CollisionShape* ColliderComponent::AddShape(CollisionShape shape) {
    m_shapes.emplace_back(shape);
    return &m_shapes.back();
}

void CollisionShape::Destroy() {
    if (m_collider) {
        LIL_LOG_TRACE("CollisionShape::Destroy()");
        m_collider->getBody()->removeCollider(m_collider);
        m_collider = nullptr;
    }
}

void CollisionShape::Create(rc::RigidBody *body) {
    rc::CollisionShape* shape = nullptr;
    switch (m_type) {
    case ::CollisionShapeType::SPHERE:
        shape =  Lil::Physics().GetCommon().createSphereShape(m_radius);
        break;
    
    case ::CollisionShapeType::BOX:
        shape =  Lil::Physics().GetCommon().createBoxShape(RcVector3(m_half_extends));
        break;
    
    case ::CollisionShapeType::HEIGHTMAP:
        if (Lil::Resources().TextureExists(m_heightmap_texture_key)) {
            Image image = LoadImageFromTexture(*Lil::Resources().GetTexture(m_heightmap_texture_key));
            shape = CreateHeightmapShape(image, Vector3{1.0f, 1.0f, 1.0f});
            UnloadImage(image);
        }
        break;
    }
    if (shape) m_collider = body->addCollider(shape, rc::Transform(RcVector3(m_local_position), RcQuaternion(m_local_rotation)));
}

void CollisionShape::Update(rc::RigidBody *body) {
    if (!m_collider) {
        LIL_LOG_TRACE("Creating shape");
        Create(body);
        LIL_LOG_TRACE("Creating shape DONE");
    }
    else {
        bool type_changed = false;
        if (rc::SphereShape* sphere = dynamic_cast<rc::SphereShape*>(m_collider->getCollisionShape())) {
            if (m_type == ::CollisionShapeType::SPHERE) {
                LIL_LOG_TRACE("Updating sphere shape params");
                sphere->setRadius(m_radius);
            }
            else type_changed = true;
        }
        else if (rc::BoxShape* box = dynamic_cast<rc::BoxShape*>(m_collider->getCollisionShape())) {
            if (m_type == ::CollisionShapeType::BOX) {
                LIL_LOG_TRACE("Updating box shape params");
                box->setHalfExtents(RcVector3(m_half_extends));
            }
            else type_changed = true;
        }
        else if (rc::HeightFieldShape* heightmap = dynamic_cast<rc::HeightFieldShape*>(m_collider->getCollisionShape())) {
            if (m_type == ::CollisionShapeType::HEIGHTMAP) {
                LIL_LOG_TRACE("Updating heightmap shape params");
                heightmap->setScale(RcVector3(m_map_size*Vector3{1.0f/128.0f, 1.0f/20.0f, 1.0f/128.0f}));
                LIL_LOG_TRACE("Updating heightmap shape params DONE");
            }
            else type_changed = true;
        }

        if (type_changed) {
            LIL_LOG_TRACE("Shape type changed");
            Destroy();
            Create(body);
        }
        else if (m_collider) {
            LIL_LOG_TRACE("Updating collider local transform");
            std::cout << m_collider << std::endl;
            std::cout << m_collider->getBody() << std::endl;
            std::cout << m_collider->getCollisionShape() << std::endl;
            if (m_type != ::CollisionShapeType::HEIGHTMAP) {
                m_collider->setLocalToBodyTransform(rc::Transform(RcVector3(m_local_position), RcQuaternion(m_local_rotation)));
            }
            LIL_LOG_TRACE("Updating collider local transform DONE");
        }
    }
}