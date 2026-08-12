#ifndef FOG_FRAG_H
#define FOG_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char FOG_FRAG[] =
    "#version 330 core\n#define V_PROJ_PERSP 0\n#define V_PROJ_ORTHO 1\nstruct View{vec3 position;mat4 view;mat4 invView;mat4 proj;mat4 invProj;mat4 viewProj;int projMode;float aspect;float near;float far;};layout(std140)uniform ViewBlock{View uView;\n};\n#define FOG_DISABLED 0\n#define FOG_LINEAR 1\n#define FOG_EXP2 2\n#define FOG_EXP 3\n#define BLOOM_MIX 1\n#define BLOOM_ADDITIVE 2\n#define BLOOM_SCREEN 3\n#define TONEMAP_LINEAR 0\n#define TONEMAP_REINHARD 1\n#define TONEMAP_FILMIC 2\n#define TONEMAP_ACES 3\n#define TONEMAP_AGX 4\nstruct FX_Ssao{int sampleCount;float intensity;float power;float ssMaxRadius;float radius;float bias;bool enabled;};struct FX_Ssil{int sampleCount;float giIntensity;float aoIntensity;float aoPower;float ssMaxRadius;float radius;float bias;bool enabled;};struct FX_Ssgi{int sliceCount;float edgeFade;float distanceFalloff;float normalRejection;float intensity;bool enabled;};struct FX_Ssr{int maxRaySteps;int binarySteps;float stepSize;float thickness;float maxDistance;float edgeFade;bool enabled;};struct FX_Fog{vec4 color;float start;float end;float density;float skyAffect;int mode;};struct FX_VFog{vec4 scatteringColor;vec4 emissionColor;float scatteringDensity;float absortionDensity;float anisotropy;float emissionEnergy;float skyAffect;float length;float stepSize;bool enabled;};struct FX_Dof{float focusPoint;float focusScale;float nearScale;float maxBlurSize;int mode;};struct FX_Bloom{vec4 prefilter;float intensity;int mode;};struct FX_Tonemap{float exposure;float white;int mode;};struct FX_Bcs{float brightness;float contrast;float saturation;};layout(std140)uniform FxBlock{FX_Ssao uSsao;FX_Ssil uSsil;FX_Ssgi uSsgi;FX_Ssr uSsr;FX_Fog uFog;FX_VFog uVFog;FX_Dof uDof;FX_Bloom uBloom;FX_Tonemap uTonemap;FX_Bcs uBcs;};float FogFactorLinear(float dist,float start,float end){return 1.-clamp((end-dist)/(end-start),0.,1.);}float FogFactorExp2(float dist,float density){const float LOG2=-1.442695;float d=density*dist;return 1.-clamp(exp2(d*d*LOG2),0.,1.);}float FogFactorExp(float dist,float density){return 1.-clamp(exp(-density*dist),0.,1.);}float FogFactor(float dist){if(uFog.mode==FOG_LINEAR)return FogFactorLinear(dist,uFog.start,uFog.end);if(uFog.mode==FOG_EXP2)return FogFactorExp2(dist,uFog.density);if(uFog.mode==FOG_EXP)return FogFactorExp(dist,uFog.density);return 0.;}vec4 FogColorAlpha(float depth,float far){return vec4(uFog.color.rgb,(depth>=far)?uFog.skyAffect:FogFactor(depth));}vec3 FogColorMix(vec3 color,float depth){return mix(color,uFog.color.rgb,FogFactor(depth));}vec4 FogColorMix(vec4 color,float depth){return vec4(mix(color.rgb,uFog.color.rgb,FogFactor(depth)),color.a);}noperspective in vec2 vTexCoord;uniform sampler2D uDepthTex;out vec4 FragColor;void main(){float depth=texelFetch(uDepthTex,ivec2(gl_FragCoord.xy),0).r;FragColor=FogColorAlpha(depth,uView.far);}"
;

#define FOG_FRAG_SIZE 2820

#ifdef __cplusplus
}
#endif

#endif // FOG_FRAG_H
