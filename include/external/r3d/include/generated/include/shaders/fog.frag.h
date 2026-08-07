#ifndef FOG_FRAG_H
#define FOG_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char FOG_FRAG[] =
    "#version 330 core\n#define FOG_DISABLED 0\n#define FOG_LINEAR 1\n#define FOG_EXP2 2\n#define FOG_EXP 3\nstruct Fog{vec3 color;float start;float end;float density;float skyAffect;int mode;};layout(std140)uniform FogBlock{Fog uFog;};float FogFactorLinear(float dist,float start,float end){return 1.-clamp((end-dist)/(end-start),0.,1.);}float FogFactorExp2(float dist,float density){const float LOG2=-1.442695;float d=density*dist;return 1.-clamp(exp2(d*d*LOG2),0.,1.);}float FogFactorExp(float dist,float density){return 1.-clamp(exp(-density*dist),0.,1.);}float FogFactor(float dist){if(uFog.mode==FOG_LINEAR)return FogFactorLinear(dist,uFog.start,uFog.end);if(uFog.mode==FOG_EXP2)return FogFactorExp2(dist,uFog.density);if(uFog.mode==FOG_EXP)return FogFactorExp(dist,uFog.density);return 0.;}vec4 FogColorAlpha(float dist){return vec4(uFog.color,FogFactor(dist));}vec3 FogColorMix(vec3 color,float dist){return mix(color,uFog.color,FogFactor(dist));}vec4 FogColorMix(vec4 color,float dist){return vec4(mix(color.rgb,uFog.color,FogFactor(dist)),color.a);}vec3 FogSkyMix(vec3 sky){if(uFog.mode==FOG_DISABLED)return sky;return mix(sky,uFog.color,uFog.skyAffect);}noperspective in vec2 vTexCoord;uniform sampler2D uDepthTex;out vec4 FragColor;void main(){float depth=texelFetch(uDepthTex,ivec2(gl_FragCoord.xy),0).r;FragColor=FogColorAlpha(depth);}"
;

#define FOG_FRAG_SIZE 1339

#ifdef __cplusplus
}
#endif

#endif // FOG_FRAG_H
