#include "utils/ColliderHelper.hpp"
#include "LilEngine.hpp"

void AddBoundsBoxColliderToBody(rc::RigidBody *body, const Model &model) {
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
    rc::BoxShape* boxShape = Lil::Physics().GetCommon().createBoxShape(halfExtents);

    // Attach the box as a collider using a local transform
    rc::Transform localTransform{center, rc::Quaternion::identity()};
    body->addCollider(boxShape, localTransform);
}

void AddBoundsBoxColliderToBodyMulty(rc::RigidBody *body, const Model &model) {
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
        rc::BoxShape* boxShape = Lil::Physics().GetCommon().createBoxShape(halfExtents);

        // Attach the box as a collider using a local transform
        rc::Transform localTransform{center, rc::Quaternion::identity()};
        body->addCollider(boxShape, localTransform);
    }
}

void AddBoundingSphereColliderToBody(rc::RigidBody *body, const Model &model) {
    if (model.meshCount == 0) return;

    float max_rad = MaxRad(model);

    // Create the box shape using half-extents
    rc::SphereShape* sphereShape = Lil::Physics().GetCommon().createSphereShape(max_rad);

    // Attach the box as a collider using a local transform
    rc::Transform localTransform{rc::Vector3{0, 0, 0}, rc::Quaternion::identity()};
    body->addCollider(sphereShape, localTransform);
}

reactphysics3d::HeightFieldShape *CreateHeightmapShape(Image heightmap_image, Vector3 scale) {
    int nbColumns = heightmap_image.width;
    int nbRows = heightmap_image.height;

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
    rc::HeightField* heightField = Lil::Physics().GetCommon().createHeightField(
        nbColumns,
        nbRows,
        heights.data(),
        rc::HeightField::HeightDataType::HEIGHT_FLOAT_TYPE,
        messages
    );

    rc::HeightFieldShape* shape = Lil::Physics().GetCommon().createHeightFieldShape(
        heightField,
        rc::Vector3(scale.x/128, scale.y/20, scale.z/128)  // scaling for x,y,z
    );
    return shape;
}
// reactphysics3d::Collider *AddHeightmapCollider(Image heightmap_image, Vector3 scale, rc::RigidBody *body)
// {
//     int nbColumns = heightmap_image.width;
//     int nbRows = heightmap_image.height;

//     std::vector<float> heights(nbColumns * nbRows, 0.0f);
//     Color* pixels = LoadImageColors(heightmap_image);
//     for (int y = 0; y < nbRows; y++) {
//         for (int x = 0; x < nbColumns; x++) {
//             int idx = y * nbColumns + x;
//             float gray = pixels[idx].r / 255.0f; // assuming grayscale image
//             heights[idx] = gray * 20.0f;         // scale heights (0..20 units)
//         }
//     }
//     UnloadImageColors(pixels);


//     std::vector<rc::Message> messages;

//     // Create the HeightField object
//     rc::HeightField* heightField = Lil::Physics().GetCommon().createHeightField(
//         nbColumns,
//         nbRows,
//         heights.data(),
//         rc::HeightField::HeightDataType::HEIGHT_FLOAT_TYPE,
//         messages
//     );

//     rc::HeightFieldShape* shape = Lil::Physics().GetCommon().createHeightFieldShape(
//         heightField,
//         rc::Vector3(scale.x/128, scale.y/20, scale.z/128)  // scaling for x,y,z
//     );

//     return body->addCollider(shape, rc::Transform::identity());
// }
