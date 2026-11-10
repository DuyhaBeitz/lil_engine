#include "Environment.hpp"
#include <lil3d/lil3d.h>

// Supported: background colour/energy, sky (cubemap), ambient colour/energy.
// Everything else in Environment (ssao, ssil, ssgi, ssr, fog, volumetric fog, dof, bloom,
// auto exposure, tonemap, colour grading, sky blur/rotation) has no equivalent in the
// backend; those fields are kept so existing code and saved scenes still work, but ignored.

// The sky cubemap currently handed to the backend (owned here, released on replace).
static LIL3D_Cubemap s_sky = {};

static void ReleaseSky(LIL3D_Environment* env) {
    if (s_sky.texture != 0) LIL3D_UnloadCubemap(s_sky);
    s_sky = LIL3D_Cubemap{};
    env->background.sky = LIL3D_Cubemap{};
    env->ambient.fromSky = false;
}

void Environment::Reset() { *this = Environment(); }

void Environment::Update() {
    LIL3D_Environment* env = LIL3D_GetEnvironment();

    const bool sky_changed = !(background.sky_texture == m_applied_sky);
    m_applied_sky = background.sky_texture;
    const bool sky_exists = Lil::Assets::TextureExists(m_applied_sky);

    // --- background / sky ---
    env->background.color  = background.color;
    env->background.energy = background.energy;

    if (sky_exists && sky_changed) {
        Image image = Lil::Assets::GetTextureImage(background.sky_texture);
        LIL3D_Cubemap cubemap = LIL3D_LoadCubemapFromImage(image, LIL3D_CUBEMAP_LAYOUT_AUTO_DETECT);
        if (cubemap.texture != 0) {
            ReleaseSky(env);
            s_sky = cubemap;
            env->background.sky = cubemap;
            env->ambient.fromSky = true;      // ambient light + reflections come from the sky
        }
    } else if (!sky_exists && s_sky.texture != 0) {
        ReleaseSky(env);                      // sky texture was removed / environment reset
    }

    // --- ambient ---
    env->ambient.color  = ambient.color;
    env->ambient.energy = ambient.energy;
}
