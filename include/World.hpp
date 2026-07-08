#pragma once

#include "CommonIncludes.hpp"
#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"

#include "ResourceManager.hpp"

#include <vector>

inline void AddBoundsBoxColliderToBody(rc::PhysicsCommon& physicsCommon, rc::RigidBody* body, const Model& model) {
    if (model.meshCount == 0) return;

    // Compute axis-aligned bounding box (AABB) of all meshes in the model
    BoundingBox sceneBB {
        GetMeshBoundingBox(model.meshes[0])
    };
    for (int i = 1; i < model.meshCount; i++)
    {
        sceneBB = {
            Vector3Min(sceneBB.min, GetMeshBoundingBox(model.meshes[i]).min),
            Vector3Max(sceneBB.max, GetMeshBoundingBox(model.meshes[i]).max)
        };
    }

    // Convert Raylib's Vector3 to ReactPhysics3D Vector3
    rc::Vector3 minPt{sceneBB.min.x, sceneBB.min.y, sceneBB.min.z};
    rc::Vector3 maxPt{sceneBB.max.x, sceneBB.max.y, sceneBB.max.z};

    // Compute center and half-extents
    rc::Vector3 halfExtents = (maxPt - minPt) * 0.5f;
    rc::Vector3 center = (maxPt + minPt) * 0.5f;

    // Create the box shape using half-extents
    rc::BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);

    // Attach the box as a collider using a local transform
    rc::Transform localTransform{center, rc::Quaternion::identity()};
    body->addCollider(boxShape, localTransform);
}

inline void AddBoundsBoxColliderToBodyMulty(rc::PhysicsCommon& physicsCommon, rc::RigidBody* body, const Model& model) {
    if (model.meshCount == 0) return;

    // Compute axis-aligned bounding box (AABB) of all meshes in the model

    for (int i = 0; i < model.meshCount; i++)
    {
        BoundingBox sceneBB {
            GetMeshBoundingBox(model.meshes[i])
        };

        // Convert Raylib's Vector3 to ReactPhysics3D Vector3
        rc::Vector3 minPt{sceneBB.min.x, sceneBB.min.y, sceneBB.min.z};
        rc::Vector3 maxPt{sceneBB.max.x, sceneBB.max.y, sceneBB.max.z};

        // Compute center and half-extents
        rc::Vector3 halfExtents = (maxPt - minPt) * 0.5f;
        rc::Vector3 center = (maxPt + minPt) * 0.5f;

        // Create the box shape using half-extents
        rc::BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);

        // Attach the box as a collider using a local transform
        rc::Transform localTransform{center, rc::Quaternion::identity()};
        body->addCollider(boxShape, localTransform);
    }
}

inline void AddBoundingSphereColliderToBody(rc::PhysicsCommon& physicsCommon, rc::RigidBody* body, const Model& model) {
    if (model.meshCount == 0) return;

    float max_rad = MaxRad(model);

    // Create the box shape using half-extents
    rc::SphereShape* sphereShape = physicsCommon.createSphereShape(max_rad);

    // Attach the box as a collider using a local transform
    rc::Transform localTransform{rc::Vector3{0, 0, 0}, rc::Quaternion::identity()};
    body->addCollider(sphereShape, localTransform);
}

inline void AddHeightmapCollider(Image heightmap_image, Vector3 scale, rc::PhysicsCommon& physicsCommon, rc::RigidBody* body) {
    int nbColumns = heightmap_image.width;
    int nbRows = heightmap_image.height;

    // Example: flat ground (all zeros)
    std::vector<float> heights(nbColumns * nbRows, 0.0f);
    Color* pixels = LoadImageColors(heightmap_image);
    for (int y = 0; y < nbRows; y++) {
        for (int x = 0; x < nbColumns; x++) {
            int idx = y * nbColumns + x;
            float gray = pixels[idx].r / 255.0f; // assuming grayscale image
            heights[idx] = gray * 20.0f;         // scale heights (0..20 units)
        }
    }
    UnloadImageColors(pixels);


    std::vector<rc::Message> messages;

    // Create the HeightField object
    rc::HeightField* heightField = physicsCommon.createHeightField(
        nbColumns,
        nbRows,
        heights.data(),
        rc::HeightField::HeightDataType::HEIGHT_FLOAT_TYPE,
        messages
    );

    rc::HeightFieldShape* shape = physicsCommon.createHeightFieldShape(
        heightField,
        rc::Vector3(scale.x/128, scale.y/20, scale.z/128)  // scaling for x,y,z
    );

    body->addCollider(shape, rc::Transform::identity());
}


class Actor {
public:
    Actor(rc::RigidBody* body) 
    : m_body(body) {
    }

    void SetModel(std::string model_key, Vector3 local_offset = {0, 0, 0}) {
        m_model_key = model_key;
        m_model_local_offset = local_offset;
    }
    
    void Draw() {
        float angle;
        Vector3 axis = GetAxisAngle(angle);
        Model* m = GetModel();
        if (m) DrawModelEx(*m, GetPosition()+m_model_local_offset, axis, angle, Vector3{1,1,1}, RAYWHITE);
        else DrawSphere(GetPosition(), 2.f, RAYRED);
    }

    Vector3 GetAxisAngle(float& angle) {
        const rc::Transform& transform = m_body->getTransform();
        const rc::Quaternion& q = transform.getOrientation();
        return QuaternionToAxis(q, angle);
    }

    Vector3 GetPosition() {
        const rc::Transform& transform = m_body->getTransform();
        const rc::Vector3& position = transform.getPosition();
        return Vector3{position.x, position.y, position.z};
    }

    void SetPosition(Vector3 position) {
        auto transform = m_body->getTransform();
        transform.setPosition(rc::Vector3{position.x, position.y, position.z});
        m_body->setTransform(transform);
    }

    rc::RigidBody* GetBody() { return m_body; }
    Model* GetModel() { return ResourceManager::Get().GetModel(m_model_key); }

    std::string& ModelKey() {return m_model_key;}

private:
    rc::RigidBody* m_body;
    std::string m_model_key;
    Vector3 m_model_local_offset;
};

class World {
public:
    World()
    : m_world(m_physicsCommon.createPhysicsWorld())
    {
    }

    std::shared_ptr<Actor> AddActor(rc::BodyType bodyType = rc::BodyType::DYNAMIC) {
        auto actor = std::make_shared<Actor>(m_world->createRigidBody(rc::Transform{}));
        actor->GetBody()->setType(bodyType);
        m_actors[std::to_string(m_actors.size())] = actor;
        return actor;
    }

    std::shared_ptr<Actor> AddHeightmapActor(Texture2D texture, Vector3 map_size) {
        Image heightmap_image = LoadImageFromTexture(texture);
        
        ResourceManager::Get().AddModel("heightmap", HeightmapModel(heightmap_image, map_size));

        auto map = AddActor(rc::BodyType::STATIC);
        map->SetModel("heightmap", Vector3{-map_size.x/2, -map_size.y/2, -map_size.z/2});
        AddHeightmapCollider(heightmap_image, map_size, m_physicsCommon, map->GetBody());
        map->SetPosition({0, 0, 0});
        auto map_body = map->GetBody();

        UnloadImage(heightmap_image);
        return map;
    }

    void Draw() {
        for (auto& [key, actor] : m_actors) {
            actor->Draw();
        }
    }

    void Update() {
        float time_step = GetFrameTime()*m_simulation_speed;
        if (m_simulation_going && time_step > 0.0) m_world->update(time_step);

        // IF YOU WANT TO DRAW DEBUG WHILE PAUSING THE SIMULATION, SET TIMESTEP TO 0 (INSTEAD OF NOT CALLING UPDATE AT ALL)
        // WITHOUT UPDATING THE PHYSICS WORLD DEBUG DRAW IS VERY SLOW

        // ACTUALLY, PROVIDING 0 IS ILLEGAL HERE, SO
        // TODO: FIX PHYSICS DEBUG SLOW WHEN NO UPDATING
    }

    void DebugDraw() {
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

    rc::PhysicsCommon& GetPhysicsCommon() { return m_physicsCommon; }
    rc::PhysicsWorld* GetPhysicsWorld() { return m_world; }

    void ToggleSimulationGoing() { m_simulation_going = !m_simulation_going; }

    bool m_simulation_going = false;
    float m_simulation_speed = 1.f;
    bool m_physics_debug = false;

private:
    rc::PhysicsCommon m_physicsCommon;
    rc::PhysicsWorld* m_world;
    std::unordered_map<std::string, std::shared_ptr<Actor>> m_actors;
};