#pragma once

#include <string>
#include <raylib.h>
#include "Reflection.hpp"
#include "ReflAttributes.hpp"
#include "Serialization.hpp"
#include "Assets/Texture.hpp"

enum class FogMode     : int { Disabled = 0, Linear, Exp2, Exp };
enum class DofMode     : int { Disabled = 0, Enabled };
enum class BloomMode   : int { Disabled = 0, Mix, Additive, Screen };
enum class TonemapMode : int { Linear = 0, Reinhard, Filmic, Aces, Agx };

struct EnvBackground {
    Color      color = {0, 0, 0, 255};
    float      energy = 1.0f;
    float      skyBlur = 0.0f;
    Lil::Assets::Texture sky_texture;
    Quaternion rotation = {0, 0, 0, 1};
};
LIL_REFLECT(EnvBackground, bases<>,
    field(color), field(energy), field(skyBlur), field(sky_texture), field(rotation)
)

struct EnvFog {
    int   mode = int(FogMode::Disabled);
    Color color = {200, 200, 200, 255};
    float start = 1.0f, end = 100.0f, density = 0.01f, skyAffect = 0.5f;
};
LIL_REFLECT(EnvFog, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"FOG_DISABLED", int(FogMode::Disabled)},
        EnumAttrItem{"FOG_LINEAR",   int(FogMode::Linear)},
        EnumAttrItem{"FOG_EXP2",     int(FogMode::Exp2)},
        EnumAttrItem{"FOG_EXP",      int(FogMode::Exp)} }),
    field(color), field(start), field(end), field(density), field(skyAffect)
)

struct EnvDoF {
    int   mode = int(DofMode::Disabled);
    float focusPoint = 10.0f, focusScale = 1.0f, nearScale = 1.0f, maxBlurSize = 20.0f;
};
LIL_REFLECT(EnvDoF, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"DOF_DISABLED", int(DofMode::Disabled)},
        EnumAttrItem{"DOF_ENABLED",  int(DofMode::Enabled)} }),
    field(focusPoint), field(focusScale), field(nearScale), field(maxBlurSize)
)

struct EnvBloom {
    int   mode = int(BloomMode::Disabled);
    float levels = 0.5f, intensity = 0.05f, threshold = 0.0f, softThreshold = 0.5f, filterRadius = 1.0f;
};
LIL_REFLECT(EnvBloom, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"BLOOM_DISABLED", int(BloomMode::Disabled)},
        EnumAttrItem{"BLOOM_MIX",      int(BloomMode::Mix)},
        EnumAttrItem{"BLOOM_ADDITIVE", int(BloomMode::Additive)},
        EnumAttrItem{"BLOOM_SCREEN",   int(BloomMode::Screen)} }),
    field(levels), field(intensity), field(threshold), field(softThreshold), field(filterRadius)
)

struct EnvTonemap {
    int   mode = int(TonemapMode::Linear);
    float exposure = 1.0f, white = 1.0f;
};
LIL_REFLECT(EnvTonemap, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"TONEMAP_LINEAR",   int(TonemapMode::Linear)},
        EnumAttrItem{"TONEMAP_REINHARD", int(TonemapMode::Reinhard)},
        EnumAttrItem{"TONEMAP_FILMIC",   int(TonemapMode::Filmic)},
        EnumAttrItem{"TONEMAP_ACES",     int(TonemapMode::Aces)},
        EnumAttrItem{"TONEMAP_AGX",      int(TonemapMode::Agx)} }),
    field(exposure), field(white)
)

struct EnvAmbient {
    Color color = {100, 100, 100, 255};
    float energy = 1.0f;
};
LIL_REFLECT(EnvAmbient, bases<>, field(color), field(energy))

struct EnvSSAO {
    int   sampleCount = 16;
    float intensity = 1.0f, power = 1.0f, maxRadius = 1.0f, radius = 0.5f, bias = 0.02f;
    bool  enabled = false;
};
LIL_REFLECT(EnvSSAO, bases<>,
    field(sampleCount), field(intensity), field(power), field(maxRadius),
    field(radius), field(bias), field(enabled))

struct EnvSSIL {
    int   sampleCount = 8;
    float giIntensity = 1.0f, aoIntensity = 1.0f, aoPower = 1.0f, maxRadius = 1.0f, radius = 0.5f, bias = 0.02f;
    bool  enabled = false;
};
LIL_REFLECT(EnvSSIL, bases<>,
    field(sampleCount), field(giIntensity), field(aoIntensity), field(aoPower),
    field(maxRadius), field(radius), field(bias), field(enabled))

struct EnvSSGI {
    int   sliceCount = 2;
    float edgeFade = 0.1f, distanceFalloff = 1.0f, normalRejection = 1.0f, intensity = 1.0f;
    int   denoiseSteps = 2;
    bool  enabled = false;
};
LIL_REFLECT(EnvSSGI, bases<>,
    field(sliceCount), field(edgeFade), field(distanceFalloff), field(normalRejection),
    field(intensity), field(denoiseSteps), field(enabled))

struct EnvSSR {
    int   maxRaySteps = 64, binarySteps = 8;
    float stepSize = 0.1f, thickness = 0.1f, maxDistance = 50.0f, edgeFade = 0.1f;
    bool  enabled = false;
};
LIL_REFLECT(EnvSSR, bases<>,
    field(maxRaySteps), field(binarySteps), field(stepSize), field(thickness),
    field(maxDistance), field(edgeFade), field(enabled))

struct EnvVolumetricFog {
    float scatteringDensity = 0.1f;
    float absortionDensity = 0.0f;            // sic: spelling kept so saved field order/names match
    Color scatteringColor = {255, 255, 255, 255};
    float anisotropy = 0.0f;
    Color emissionColor = {0, 0, 0, 255};
    float emissionEnergy = 0.0f, skyAffect = 0.5f, length = 64.0f, stepSize = 1.0f;
    bool  enabled = false;
};
LIL_REFLECT(EnvVolumetricFog, bases<>,
    field(scatteringDensity), field(absortionDensity), field(scatteringColor), field(anisotropy),
    field(emissionColor), field(emissionEnergy), field(skyAffect), field(length),
    field(stepSize), field(enabled))

struct EnvAutoExposure {
    float minEV = -5.0f, maxEV = 5.0f, exposureCompensation = 0.0f,
          adaptationToBright = 3.0f, adaptationToDark = 1.0f;
    bool  enabled = false;
};
LIL_REFLECT(EnvAutoExposure, bases<>,
    field(minEV), field(maxEV), field(exposureCompensation),
    field(adaptationToBright), field(adaptationToDark), field(enabled))

struct EnvColor {
    float brightness = 1.0f, contrast = 1.0f, saturation = 1.0f;
};
LIL_REFLECT(EnvColor, bases<>, field(brightness), field(contrast), field(saturation))

class Environment : public Reflectable {
public:
    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()

    void Update();
    void Reset();

    EnvBackground    background;
    EnvAmbient       ambient;
    EnvSSAO          ssao;
    EnvSSIL          ssil;
    EnvSSGI          ssgi;
    EnvSSR           ssr;
    EnvFog           fog;
    EnvVolumetricFog volumetricFog;
    EnvDoF           dof;
    EnvBloom         bloom;
    EnvAutoExposure  autoExposure;
    EnvTonemap       tonemap;
    EnvColor         color;

private:
    Lil::Assets::Texture m_applied_sky;
};
LIL_REFLECT(Environment, bases<>,
    field(background), field(ambient), field(ssao), field(ssil), field(ssgi), field(ssr),
    field(fog), field(volumetricFog), field(dof), field(bloom), field(autoExposure),
    field(tonemap), field(color)
)
LIL_SER_BEGIN(Environment)
    LIL_SER_FIELD(background)   LIL_SER_FIELD(ambient)   LIL_SER_FIELD(ssao)
    LIL_SER_FIELD(ssil)         LIL_SER_FIELD(ssgi)      LIL_SER_FIELD(ssr)
    LIL_SER_FIELD(fog)          LIL_SER_FIELD(volumetricFog) LIL_SER_FIELD(dof)
    LIL_SER_FIELD(bloom)        LIL_SER_FIELD(autoExposure)  LIL_SER_FIELD(tonemap)
    LIL_SER_FIELD(color)
LIL_SER_END()

#define SER(T) template <class Archive> void serialize(Archive& ar, T& v)
namespace cereal {
    SER(EnvBackground)      { ar(v.color, v.energy, v.rotation, v.sky_texture, v.skyBlur); }
    SER(EnvDoF)             { ar(v.focusPoint, v.focusScale, v.maxBlurSize, v.mode, v.nearScale); }
    SER(EnvBloom)           { ar(v.filterRadius, v.intensity, v.levels, v.mode, v.softThreshold, v.threshold); }
    SER(EnvTonemap)         { ar(v.exposure, v.mode, v.white); }
    SER(EnvFog)             { ar(v.color, v.density, v.end, v.mode, v.skyAffect, v.start); }
    SER(EnvAmbient)         { ar(v.color, v.energy); }
    SER(EnvSSAO)            { ar(v.bias, v.enabled, v.intensity, v.maxRadius, v.power, v.radius, v.sampleCount); }
    SER(EnvSSIL)            { ar(v.aoIntensity, v.aoPower, v.bias, v.enabled, v.giIntensity, v.maxRadius, v.radius, v.sampleCount); }
    SER(EnvSSGI)            { ar(v.denoiseSteps, v.distanceFalloff, v.edgeFade, v.enabled, v.intensity, v.normalRejection, v.sliceCount); }
    SER(EnvSSR)             { ar(v.binarySteps, v.edgeFade, v.enabled, v.maxDistance, v.maxRaySteps, v.stepSize, v.thickness); }
    SER(EnvVolumetricFog)   { ar(v.absortionDensity, v.anisotropy, v.emissionColor, v.emissionEnergy, v.enabled, v.length, v.scatteringColor, v.scatteringDensity, v.skyAffect, v.stepSize); }
    SER(EnvAutoExposure)    { ar(v.adaptationToBright, v.adaptationToDark, v.enabled, v.exposureCompensation, v.maxEV, v.minEV); }
    SER(EnvColor)           { ar(v.brightness, v.contrast, v.saturation); }
}
#undef SER