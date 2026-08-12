#ifndef BLOOM_FRAG_H
#define BLOOM_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char BLOOM_FRAG[] =
    "#version 330 core\n#define FOG_DISABLED 0\n#define FOG_LINEAR 1\n#define FOG_EXP2 2\n#define FOG_EXP 3\n#define BLOOM_MIX 1\n#define BLOOM_ADDITIVE 2\n#define BLOOM_SCREEN 3\n#define TONEMAP_LINEAR 0\n#define TONEMAP_REINHARD 1\n#define TONEMAP_FILMIC 2\n#define TONEMAP_ACES 3\n#define TONEMAP_AGX 4\nstruct FX_Ssao{int sampleCount;float intensity;float power;float ssMaxRadius;float radius;float bias;bool enabled;};struct FX_Ssil{int sampleCount;float giIntensity;float aoIntensity;float aoPower;float ssMaxRadius;float radius;float bias;bool enabled;};struct FX_Ssgi{int sliceCount;float edgeFade;float distanceFalloff;float normalRejection;float intensity;bool enabled;};struct FX_Ssr{int maxRaySteps;int binarySteps;float stepSize;float thickness;float maxDistance;float edgeFade;bool enabled;};struct FX_Fog{vec4 color;float start;float end;float density;float skyAffect;int mode;};struct FX_VFog{vec4 scatteringColor;vec4 emissionColor;float scatteringDensity;float absortionDensity;float anisotropy;float emissionEnergy;float skyAffect;float length;float stepSize;bool enabled;};struct FX_Dof{float focusPoint;float focusScale;float nearScale;float maxBlurSize;int mode;};struct FX_Bloom{vec4 prefilter;float intensity;int mode;};struct FX_Tonemap{float exposure;float white;int mode;};struct FX_Bcs{float brightness;float contrast;float saturation;};layout(std140)uniform FxBlock{FX_Ssao uSsao;FX_Ssil uSsil;FX_Ssgi uSsgi;FX_Ssr uSsr;FX_Fog uFog;FX_VFog uVFog;FX_Dof uDof;FX_Bloom uBloom;FX_Tonemap uTonemap;FX_Bcs uBcs;};noperspective in vec2 vTexCoord;out vec3 FragColor;uniform sampler2D uSceneTex;uniform sampler2D uBloomTex;void main(){vec3 color=texture(uSceneTex,vTexCoord).rgb;vec3 bloom=texture(uBloomTex,vTexCoord).rgb;if(uBloom.mode==BLOOM_MIX){color=mix(color,bloom,uBloom.intensity);}else if(uBloom.mode==BLOOM_ADDITIVE){color+=bloom*uBloom.intensity;}else if(uBloom.mode==BLOOM_SCREEN){bloom=clamp(bloom*uBloom.intensity,vec3(0.),vec3(1.));color=max((color+bloom)-(color*bloom),vec3(0.));}FragColor=vec3(color);}"
;

#define BLOOM_FRAG_SIZE 2023

#ifdef __cplusplus
}
#endif

#endif // BLOOM_FRAG_H
