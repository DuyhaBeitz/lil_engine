#pragma once

#include "CommonIncludes.hpp"
#include "Reflection.hpp"
#include "ReflAttributes.hpp"

struct EnvBackground {
    Color color;
    float energy;
    float skyBlur;
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
        EnumAttrItem{"FOG_DISABLED", 0},
        EnumAttrItem{"FOG_LINEAR",   1},
        EnumAttrItem{"FOG_EXP2",     2},
        EnumAttrItem{"FOG_EXP",      3}
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
        EnumAttrItem{"DOF_DISABLED", 0},
        EnumAttrItem{"DOF_ENABLED", 1}
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
        EnumAttrItem{"BLOOM_DISABLED", 0},
        EnumAttrItem{"BLOOM_MIX",      1},
        EnumAttrItem{"BLOOM_ADDITIVE", 2},
        EnumAttrItem{"BLOOM_SCREEN",   3}
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
        EnumAttrItem{"TONEMAP_LINEAR",  0},
        EnumAttrItem{"TONEMAP_REINHARD",1},
        EnumAttrItem{"TONEMAP_FILMIC",  2},
        EnumAttrItem{"TONEMAP_ACES",    3},
        EnumAttrItem{"TONEMAP_AGX",     4},
    }),
    field(exposure),
    field(white)
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
    
    void Update();
    void Reset();

    // wrappers (because they hold enum or asset)

    EnvBackground       background;
    EnvDoF              dof;
    EnvBloom            bloom;
    EnvTonemap          tonemap;
    EnvFog              fog;

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