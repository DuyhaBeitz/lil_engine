#pragma once

#include "CommonIncludes.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"
#include "Serialization.hpp"

struct EnvBackground {
    Color color;
    float energy;
    float skyBlur;
    std::string old_sky_texture;
    std::string sky_texture;
    Quaternion rotation;
};
LIL_REFLECT(EnvBackground, bases<>,
    field(color),
    field(energy),
    field(skyBlur),
    field(sky_texture, TextureKeyAttribute()),
    field(rotation)
)

struct EnvFog {
    int mode;
    Color color;
    float start;
    float end;
    float density;
    float skyAffect;
};
LIL_REFLECT(EnvFog, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"FOG_DISABLED", int(R3D_FOG_DISABLED)},
        EnumAttrItem{"FOG_LINEAR",   int(R3D_FOG_LINEAR)},
        EnumAttrItem{"FOG_EXP2",     int(R3D_FOG_EXP2)},
        EnumAttrItem{"FOG_EXP",      int(R3D_FOG_EXP)}
    }),
    field(color),
    field(start),
    field(end),
    field(density),
    field(skyAffect)
)

struct EnvDoF {
    int mode;
    float focusPoint;
    float focusScale;
    float nearScale;
    float maxBlurSize;
};
LIL_REFLECT(EnvDoF, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"DOF_DISABLED", int(R3D_DOF_DISABLED)},
        EnumAttrItem{"DOF_ENABLED", int(R3D_DOF_ENABLED)}
    }),
    field(focusPoint),
    field(focusScale),
    field(nearScale),
    field(maxBlurSize)
)

struct EnvBloom {
    int mode;
    float levels;
    float intensity;
    float threshold;
    float softThreshold;
    float filterRadius;
};
LIL_REFLECT(EnvBloom, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"BLOOM_DISABLED", int(R3D_BLOOM_DISABLED)},
        EnumAttrItem{"BLOOM_MIX",      int(R3D_BLOOM_MIX)},
        EnumAttrItem{"BLOOM_ADDITIVE", int(R3D_BLOOM_ADDITIVE)},
        EnumAttrItem{"BLOOM_SCREEN",   int(R3D_BLOOM_SCREEN)}
    }),
    field(levels),
    field(intensity),
    field(threshold),
    field(softThreshold),
    field(filterRadius)
)

struct EnvTonemap {
    int mode;
    float exposure;
    float white;
};
LIL_REFLECT(EnvTonemap, bases<>,
    field(mode, EnumAttribute{
        EnumAttrItem{"TONEMAP_LINEAR",  int(R3D_TONEMAP_LINEAR)},
        EnumAttrItem{"TONEMAP_REINHARD",int(R3D_TONEMAP_REINHARD)},
        EnumAttrItem{"TONEMAP_FILMIC",  int(R3D_TONEMAP_FILMIC)},
        EnumAttrItem{"TONEMAP_ACES",    int(R3D_TONEMAP_ACES)},
        EnumAttrItem{"TONEMAP_AGX",     int(R3D_TONEMAP_AGX)},
    }),
    field(exposure),
    field(white)
)

LIL_REFLECT(R3D_EnvAmbient, bases<>,
    field(color),
    field(energy)
)
LIL_REFLECT(R3D_EnvSSAO, bases<>,
    field(sampleCount),
    field(intensity),
    field(power),
    field(maxRadius),
    field(radius),
    field(bias),
    field(enabled)
)
LIL_REFLECT(R3D_EnvSSIL, bases<>,
    field(sampleCount),
    field(giIntensity),
    field(aoIntensity),
    field(aoPower),
    field(maxRadius),
    field(radius),
    field(bias),
    field(enabled)
)
LIL_REFLECT(R3D_EnvSSGI, bases<>,
    field(sliceCount),
    field(edgeFade),
    field(distanceFalloff),
    field(normalRejection),
    field(intensity),
    field(denoiseSteps),
    field(enabled)
)
LIL_REFLECT(R3D_EnvSSR, bases<>,
    field(maxRaySteps),
    field(binarySteps),
    field(stepSize),
    field(thickness),
    field(maxDistance),
    field(edgeFade),
    field(enabled)
)
LIL_REFLECT(R3D_VolumetricFog, bases<>,
    field(scatteringDensity),
    field(absortionDensity),
    field(scatteringColor),
    field(anisotropy),
    field(emissionColor),
    field(emissionEnergy),
    field(skyAffect),
    field(length),
    field(stepSize),
    field(enabled)
)
LIL_REFLECT(R3D_EnvAutoExposure, bases<>,
    field(minEV),
    field(maxEV),
    field(exposureCompensation),
    field(adaptationToBright),
    field(adaptationToDark),
    field(enabled)
)
LIL_REFLECT(R3D_EnvColor, bases<>,
    field(brightness),
    field(contrast),
    field(saturation)
)
class Environment : public Reflectable {
private:
    void PopulateDefaultValues();

public:
    Environment() {PopulateDefaultValues();};

    LIL_REFLECTABLE()
    LIL_SERIALIZABLE()
    
    void Update();
    void Reset();

    // wrappers (because they hold enum or asset)

    EnvBackground       background;
    EnvDoF              dof;
    EnvBloom            bloom;
    EnvTonemap          tonemap;
    EnvFog              fog;

    R3D_EnvAmbient      ambient;
    R3D_EnvSSAO         ssao;
    R3D_EnvSSIL         ssil;
    R3D_EnvSSGI         ssgi;
    R3D_EnvSSR          ssr;
    R3D_VolumetricFog   volumetricFog;
    R3D_EnvAutoExposure autoExposure;
    R3D_EnvColor        color;
};
LIL_REFLECT(Environment, bases<>,
    field(background),
    field(ambient),
    field(ssao),
    field(ssil),
    field(ssgi),
    field(ssr),
    field(fog),
    field(volumetricFog),
    field(dof),
    field(bloom),
    field(autoExposure),
    field(tonemap),
    field(color)
)
LIL_SER_BEGIN(Environment)
    LIL_SER_FIELD(background)
    LIL_SER_FIELD(ambient)
    LIL_SER_FIELD(ssao)
    LIL_SER_FIELD(ssil)
    LIL_SER_FIELD(ssgi)
    LIL_SER_FIELD(ssr)
    LIL_SER_FIELD(fog)
    LIL_SER_FIELD(volumetricFog)
    LIL_SER_FIELD(dof)
    LIL_SER_FIELD(bloom)
    LIL_SER_FIELD(autoExposure)
    LIL_SER_FIELD(tonemap)
    LIL_SER_FIELD(color)
LIL_SER_END()

#define SER(typename) \
    template <class Archive> \
    void serialize(Archive& ar, typename& v)

namespace cereal {
    SER(EnvBackground) {ar(v.color, v.energy, v.rotation, v.sky_texture, v.skyBlur);}
    SER(EnvDoF) {ar(v.focusPoint, v.focusScale, v.maxBlurSize, v.mode, v.nearScale);}
    SER(EnvBloom) {ar(v.filterRadius, v.intensity, v.levels, v.mode, v.softThreshold, v.threshold);}
    SER(EnvTonemap) {ar(v.exposure, v.mode, v.white);}
    SER(EnvFog) {ar(v.color, v.density, v.end, v.mode, v.skyAffect, v.start);}
    
    SER(R3D_AmbientMap) {ar(v.flags, v.irradiance, v.prefilter);}
    SER(R3D_EnvAmbient) {ar(v.color, v.energy, v.map);}

    SER(R3D_EnvSSAO) {ar(v.bias, v.enabled, v.intensity, v.maxRadius, v.power, v.radius, v.sampleCount);}
    SER(R3D_EnvSSIL) {ar(v.aoIntensity, v.aoPower, v.bias, v.enabled, v.giIntensity, v.maxRadius, v.radius, v.sampleCount);}
    SER(R3D_EnvSSGI) {ar(v.denoiseSteps, v.distanceFalloff, v.edgeFade, v.enabled, v.intensity, v.normalRejection, v.sliceCount);}
    SER(R3D_EnvSSR) {ar(v.binarySteps, v.edgeFade, v.enabled, v.maxDistance, v.maxRaySteps, v.stepSize, v.thickness);}
    SER(R3D_VolumetricFog) {ar(v.absortionDensity, v.anisotropy, v.emissionColor, v.emissionEnergy, v.enabled, v.length, v.scatteringColor, v.scatteringDensity, v.skyAffect, v.stepSize);}
    SER(R3D_EnvAutoExposure) {ar(v.adaptationToBright, v.adaptationToDark, v.enabled, v.exposureCompensation, v.maxEV, v.minEV);}
    SER(R3D_EnvColor) {ar(v.brightness, v.contrast, v.saturation);}
}