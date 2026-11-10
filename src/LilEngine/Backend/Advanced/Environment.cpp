#include "Environment.hpp"
#include <r3d/r3d.h>

static_assert(int(FogMode::Disabled) == int(R3D_FOG_DISABLED));
static_assert(int(FogMode::Linear)   == int(R3D_FOG_LINEAR));
static_assert(int(FogMode::Exp2)     == int(R3D_FOG_EXP2));
static_assert(int(FogMode::Exp)      == int(R3D_FOG_EXP));
static_assert(int(DofMode::Disabled) == int(R3D_DOF_DISABLED));
static_assert(int(DofMode::Enabled)  == int(R3D_DOF_ENABLED));
static_assert(int(BloomMode::Disabled) == int(R3D_BLOOM_DISABLED));
static_assert(int(BloomMode::Mix)      == int(R3D_BLOOM_MIX));
static_assert(int(BloomMode::Additive) == int(R3D_BLOOM_ADDITIVE));
static_assert(int(BloomMode::Screen)   == int(R3D_BLOOM_SCREEN));
static_assert(int(TonemapMode::Linear)   == int(R3D_TONEMAP_LINEAR));
static_assert(int(TonemapMode::Reinhard) == int(R3D_TONEMAP_REINHARD));
static_assert(int(TonemapMode::Filmic)   == int(R3D_TONEMAP_FILMIC));
static_assert(int(TonemapMode::Aces)     == int(R3D_TONEMAP_ACES));
static_assert(int(TonemapMode::Agx)      == int(R3D_TONEMAP_AGX));

#define SET(path, value) R3D_ENVIRONMENT_SET(path, value)


void Environment::Reset() { *this = Environment(); }

void Environment::Update() {
    const bool sky_changed = !(background.sky_texture == m_applied_sky);
    m_applied_sky = background.sky_texture;

SET(background.color,  background.color);
    SET(background.energy, background.energy);
    SET(background.skyBlur, background.skyBlur);
    SET(background.rotation, background.rotation);

    if (sky_changed) {
        if (Lil::Assets::TextureExists(m_applied_sky)) {
            SET(background.color, background.color);
            Image image = Lil::Assets::GetTextureImage(background.sky_texture);
            R3D_Cubemap cubemap = R3D_LoadCubemapFromImage(image, R3D_CUBEMAP_LAYOUT_AUTO_DETECT);
            SET(background.sky, cubemap);
            SET(ambient.map, R3D_GenAmbientMap(cubemap, R3D_AMBIENT_ILLUMINATION | R3D_AMBIENT_REFLECTION));
        }
    }

    // --- ambient ---
    SET(ambient.color,  ambient.color);
    SET(ambient.energy, ambient.energy);

    // --- screen-space effects ---
    SET(ssao.sampleCount, ssao.sampleCount);   SET(ssao.intensity, ssao.intensity);
    SET(ssao.power, ssao.power);               SET(ssao.maxRadius, ssao.maxRadius);
    SET(ssao.radius, ssao.radius);             SET(ssao.bias, ssao.bias);
    SET(ssao.enabled, ssao.enabled);

    SET(ssil.sampleCount, ssil.sampleCount);   SET(ssil.giIntensity, ssil.giIntensity);
    SET(ssil.aoIntensity, ssil.aoIntensity);   SET(ssil.aoPower, ssil.aoPower);
    SET(ssil.maxRadius, ssil.maxRadius);       SET(ssil.radius, ssil.radius);
    SET(ssil.bias, ssil.bias);                 SET(ssil.enabled, ssil.enabled);

    SET(ssgi.sliceCount, ssgi.sliceCount);     SET(ssgi.edgeFade, ssgi.edgeFade);
    SET(ssgi.distanceFalloff, ssgi.distanceFalloff);
    SET(ssgi.normalRejection, ssgi.normalRejection);
    SET(ssgi.intensity, ssgi.intensity);       SET(ssgi.denoiseSteps, ssgi.denoiseSteps);
    SET(ssgi.enabled, ssgi.enabled);

    SET(ssr.maxRaySteps, ssr.maxRaySteps);     SET(ssr.binarySteps, ssr.binarySteps);
    SET(ssr.stepSize, ssr.stepSize);           SET(ssr.thickness, ssr.thickness);
    SET(ssr.maxDistance, ssr.maxDistance);     SET(ssr.edgeFade, ssr.edgeFade);
    SET(ssr.enabled, ssr.enabled);

    // --- fog ---
    SET(fog.mode, R3D_Fog(fog.mode));      // type names follow R3D_FOG_*; compiler will tell you if one differs
    SET(fog.color, fog.color);                 SET(fog.start, fog.start);
    SET(fog.end, fog.end);                     SET(fog.density, fog.density);
    SET(fog.skyAffect, fog.skyAffect);

    SET(volumetricFog.scatteringDensity, volumetricFog.scatteringDensity);
    SET(volumetricFog.absortionDensity,  volumetricFog.absortionDensity);
    SET(volumetricFog.scatteringColor,   volumetricFog.scatteringColor);
    SET(volumetricFog.anisotropy,        volumetricFog.anisotropy);
    SET(volumetricFog.emissionColor,     volumetricFog.emissionColor);
    SET(volumetricFog.emissionEnergy,    volumetricFog.emissionEnergy);
    SET(volumetricFog.skyAffect,         volumetricFog.skyAffect);
    SET(volumetricFog.length,            volumetricFog.length);
    SET(volumetricFog.stepSize,          volumetricFog.stepSize);
    SET(volumetricFog.enabled,           volumetricFog.enabled);

    // --- post ---
    SET(dof.mode, R3D_DoF(dof.mode));
    SET(dof.focusPoint, dof.focusPoint);       SET(dof.focusScale, dof.focusScale);
    SET(dof.nearScale, dof.nearScale);         SET(dof.maxBlurSize, dof.maxBlurSize);

    SET(bloom.mode, R3D_Bloom(bloom.mode));
    SET(bloom.levels, bloom.levels);           SET(bloom.intensity, bloom.intensity);
    SET(bloom.threshold, bloom.threshold);     SET(bloom.softThreshold, bloom.softThreshold);
    SET(bloom.filterRadius, bloom.filterRadius);

    SET(autoExposure.minEV, autoExposure.minEV);
    SET(autoExposure.maxEV, autoExposure.maxEV);
    SET(autoExposure.exposureCompensation, autoExposure.exposureCompensation);
    SET(autoExposure.adaptationToBright,   autoExposure.adaptationToBright);
    SET(autoExposure.adaptationToDark,     autoExposure.adaptationToDark);
    SET(autoExposure.enabled,              autoExposure.enabled);

    SET(tonemap.mode, R3D_Tonemap(tonemap.mode));
    SET(tonemap.exposure, tonemap.exposure);   SET(tonemap.white, tonemap.white);

    SET(color.brightness, color.brightness);
    SET(color.contrast,   color.contrast);
    SET(color.saturation, color.saturation);
}