#include "Environment.hpp"

void Environment::PopulateDefaultValues() {
    background = {
        .color = GRAY,
        .energy = 1.0f,
        .skyBlur = 0.0f,
        .sky_texture = {0},
        .rotation = {0.0f, 0.0f, 0.0f, 1.0f},
    };
    dof = {
        .mode = int(R3D_DOF_DISABLED),
        .focusPoint = 10.0f,
        .focusScale = 1.0f,
        .nearScale = 1.0f,
        .maxBlurSize = 20.0f,
    };
    bloom = {
        .mode = int(R3D_BLOOM_DISABLED),
        .levels = 0.5f,
        .intensity = 0.05f,
        .threshold = 0.0f,
        .softThreshold = 0.5f,
        .filterRadius = 1.0f,
    };
    tonemap = {
        .mode = int(R3D_TONEMAP_LINEAR),
        .exposure = 1.0f,
        .white = 1.0f,
    };
    fog = {
        .mode = int(R3D_FOG_DISABLED),
        .color = WHITE,
        .start = 1.0f,
        .end = 50.0f,
        .density = 0.05f,
        .skyAffect = 0.5f,
    };

    // ambient = {
    //     .color = BLACK,
    //     .energy = 1.0f,
    //     .map = {0},
    // };
    ssao = {
        .sampleCount = 16,
        .intensity = 1.0f,
        .power = 1.0f,
        .maxRadius = 0.2f,
        .radius = 1.0f,
        .bias = 0.03f,
        .enabled = false,
    };
    ssil = {
        .sampleCount = 16,
        .giIntensity = 1.0f,
        .aoIntensity = 1.0f,
        .aoPower = 1.0f,
        .maxRadius = 0.2f,
        .radius = 4.0f,
        .bias = 0.03f,
        .enabled = false,
    };
    ssgi = {
        .sliceCount = 4,
        .edgeFade = 0.1f,
        .distanceFalloff = 1.0f,
        .normalRejection = 0.0f,
        .intensity = 1.0f,
        .denoiseSteps = 4,
        .enabled = false,
    };
    ssr = {
        .maxRaySteps = 32,
        .binarySteps = 4,
        .stepSize = 0.125f,
        .thickness = 0.2f,
        .maxDistance = 4.0f,
        .edgeFade = 0.25f,
        .enabled = false,
    };
    volumetricFog = {
        .scatteringDensity = 0.01f,
        .absortionDensity = 0.03f,
        .scatteringColor = WHITE,
        .anisotropy = 0.5f,
        .emissionColor = WHITE,
        .emissionEnergy = 0.0f,
        .skyAffect = 0.5f,
        .length = 50.0f,
        .stepSize = 1.0f,
        .enabled = false,
    };
    autoExposure = {
        .minEV = -1.0f,
        .maxEV =  1.0f,
        .exposureCompensation = 0.0f,
        .adaptationToBright = 0.5f,
        .adaptationToDark = 1.0f,
    };
    color = {
        .brightness = 1.0f,
        .contrast = 1.0f,
        .saturation = 1.0f,
    };
}

void Environment::Update() {
    R3D_ENVIRONMENT_SET(background, R3D_EnvBackground(
        background.color,
        background.energy,
        background.skyBlur,
        {0},
        background.rotation
    ));

    R3D_ENVIRONMENT_SET(dof, R3D_EnvDoF(R3D_DoF(dof.mode), dof.focusPoint, dof.focusScale, dof.nearScale, dof.maxBlurSize));
    R3D_ENVIRONMENT_SET(bloom, R3D_EnvBloom(R3D_Bloom(bloom.mode), bloom.levels, bloom.intensity, bloom.threshold, bloom.softThreshold, bloom.filterRadius));
    R3D_ENVIRONMENT_SET(tonemap, R3D_EnvTonemap(R3D_Tonemap(tonemap.mode), tonemap.exposure, tonemap.white));
    R3D_ENVIRONMENT_SET(fog, R3D_EnvFog(R3D_Fog(fog.mode), fog.color, fog.start, fog.end, fog.density, fog.skyAffect));

    R3D_ENVIRONMENT_SET(ssao, ssao);
    R3D_ENVIRONMENT_SET(ssil, ssil);
    R3D_ENVIRONMENT_SET(ssgi, ssgi);
    R3D_ENVIRONMENT_SET(ssr, ssr);
    R3D_ENVIRONMENT_SET(volumetricFog, volumetricFog);
    R3D_ENVIRONMENT_SET(autoExposure, autoExposure);
    R3D_ENVIRONMENT_SET(color, color);
}

void Environment::Reset() {
    PopulateDefaultValues();
}