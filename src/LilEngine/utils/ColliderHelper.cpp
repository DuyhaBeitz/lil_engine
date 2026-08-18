#include "utils/ColliderHelper.hpp"
#include "utils/MeshHelper.hpp"
#include "utils/MathHelper.hpp"
#include "LilEngine.hpp"

JPH::HeightFieldShapeSettings GetHeightmapShapeSettings(const Image &img, const Vector3 scale, const Vector3 offset) {
    int sampleCount = img.width;

    if (img.width != img.height || sampleCount < 4) {
        LIL_LOG_ERROR("Heightmap texture must be square and at least 4x4!");
        return JPH::HeightFieldShapeSettings();
    }

    Color* pixels = LoadImageColors(img);
    if (!pixels) {
        LIL_LOG_ERROR("Failed to load pixel colors from heightmap image!");
        return JPH::HeightFieldShapeSettings();
    }

    std::vector<float> samples(sampleCount * sampleCount);
    for (int y = 0; y < sampleCount; ++y) {
        for (int x = 0; x < sampleCount; ++x) {
            Color color = pixels[y * sampleCount + x];
            float normalized_height = static_cast<float>(color.r) / 255.0f;
            samples[y * sampleCount + x] = normalized_height * scale.y;
        }
    }
    UnloadImageColors(pixels);

    JPH::Vec3 jph_scale(scale.x / 128.0f, 1.0f, scale.z / 128.0f);
    JPH::Vec3 jph_offset(
        -0.5f * (sampleCount - 1) * jph_scale.GetX() + offset.x,
        offset.y,
        -0.5f * (sampleCount - 1) * jph_scale.GetZ() + offset.z
    );

    return JPH::HeightFieldShapeSettings(
        samples.data(),
        jph_offset,
        jph_scale,
        static_cast<JPH::uint32>(sampleCount)
    );
}

void DrawDebugPhysicsBody(JPH::BodyID body_id) {
    JPH::BodyLockRead lock(
        Lil::Physics().GetSystem()->GetBodyLockInterface(),
        body_id
    );

    if (!lock.Succeeded()) return;

    const JPH::Body& body = lock.GetBody();

    const JPH::Shape* shape = body.GetShape();
    JPH::RMat44 comTransform = body.GetCenterOfMassTransform();
    JPH::Vec3 scale = JPH::Vec3(1.0f, 1.0f, 1.0f);

    JPH::Color bodyColor;

    if (body.IsSensor()) bodyColor = JPH::Color::sYellow;
    else if (!body.IsActive()) bodyColor = JPH::Color::sGrey;
    else {
        switch (body.GetMotionType()) {
            case JPH::EMotionType::Static:    bodyColor = JPH::Color::sDarkBlue; break;
            case JPH::EMotionType::Kinematic: bodyColor = JPH::Color::sGreen;   break;
            case JPH::EMotionType::Dynamic:   bodyColor = JPH::Color::sOrange;  break;
        }
    }

    shape->Draw(
        Lil::Physics().GetDebugRender(),
        comTransform,
        scale,
        bodyColor,
        false,
        true
    );
}
