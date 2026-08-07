#ifndef SKYBOX_FRAG_H
#define SKYBOX_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char SKYBOX_FRAG[] =
    "#version 330 core\n#define FOG_DISABLED 0\n#define FOG_LINEAR 1\n#define FOG_EXP2 2\n#define FOG_EXP 3\nstruct Fog{vec3 color;float start;float end;float density;float skyAffect;int mode;};layout(std140)uniform FogBlock{Fog uFog;};float FogFactorLinear(float dist,float start,float end){return 1.-clamp((end-dist)/(end-start),0.,1.);}float FogFactorExp2(float dist,float density){const float LOG2=-1.442695;float d=density*dist;return 1.-clamp(exp2(d*d*LOG2),0.,1.);}float FogFactorExp(float dist,float density){return 1.-clamp(exp(-density*dist),0.,1.);}float FogFactor(float dist){if(uFog.mode==FOG_LINEAR)return FogFactorLinear(dist,uFog.start,uFog.end);if(uFog.mode==FOG_EXP2)return FogFactorExp2(dist,uFog.density);if(uFog.mode==FOG_EXP)return FogFactorExp(dist,uFog.density);return 0.;}vec4 FogColorAlpha(float dist){return vec4(uFog.color,FogFactor(dist));}vec3 FogColorMix(vec3 color,float dist){return mix(color,uFog.color,FogFactor(dist));}vec4 FogColorMix(vec4 color,float dist){return vec4(mix(color.rgb,uFog.color,FogFactor(dist)),color.a);}vec3 FogSkyMix(vec3 sky){if(uFog.mode==FOG_DISABLED)return sky;return mix(sky,uFog.color,uFog.skyAffect);\n}\n#define M_PI 3.1415926535897931\n#define M_HPI 1.5707963267948966\n#define M_TAU 6.2831853071795862\n#define M_INV_PI .3183098861837907\n#define M_PHI 1.6180339887498949\n#define M_PHI_FRAC .6180339887498949\n#define M_GOLDEN_ANGLE 2.3999632297286535\nvec3 M_Rotate3D(vec3 v,vec4 q){vec3 t=2.*cross(q.xyz,v);return v+q.w*t+cross(q.xyz,t);}mat3 M_OrthonormalBasis(vec3 n){float sgn=n.z>=0.?1.:-1.;float a=-1./(sgn+n.z);float b=n.x*n.y*a;vec3 t=vec3(1.+sgn*n.x*n.x*a,sgn*b,-sgn*n.x);vec3 bt=vec3(b,sgn+n.y*n.y*a,-n.y);return mat3(t,bt,n);}vec2 M_OctahedronWrap(vec2 val){return(1.-abs(val.yx))*mix(vec2(-1.),vec2(1.),vec2(greaterThanEqual(val.xy,vec2(0.))));}vec3 M_DecodeOctahedral(vec2 encoded){encoded=encoded*2.-1.;vec3 normal;normal.z=1.-abs(encoded.x)-abs(encoded.y);normal.xy=normal.z>=0.?encoded.xy:M_OctahedronWrap(encoded.xy);return normalize(normal);}vec2 M_EncodeOctahedral(vec3 normal){normal/=abs(normal.x)+abs(normal.y)+abs(normal.z);normal.xy=normal.z>=0.?normal.xy:M_OctahedronWrap(normal.xy);normal.xy=normal.xy*.5+.5;return normal.xy;}vec3 M_NormalScale(vec3 normal,float scale){normal.xy*=scale;normal.z=sqrt(1.-clamp(dot(normal.xy,normal.xy),0.,1.));return normal;}float M_HashIGN(vec2 pos){const vec3 magic=vec3(0.06711056,0.00583715,52.9829189);return fract(magic.z*fract(dot(pos,magic.xy)));}float M_HashIGN(vec2 pos,float frame){vec3 magic=vec3(0.06711056,0.00583715,52.9829189);return fract(magic.z*fract(dot(vec3(pos,frame),magic)));}float M_HashR2(vec2 p){const vec2 k=vec2(.75487766624669276,.56984029099805327);return fract(dot(p,k));}in vec3 vViewRay;uniform samplerCube uSkyMap;uniform vec4 uRotation;uniform float uEnergy;uniform float uLod;layout(location=0)out vec3 FragColor;void main(){vec3 direction=normalize(vViewRay);direction=M_Rotate3D(direction,uRotation);FragColor=textureLod(uSkyMap,direction,uLod).rgb*uEnergy;FragColor=FogSkyMix(FragColor);}"
;

#define SKYBOX_FRAG_SIZE 3037

#ifdef __cplusplus
}
#endif

#endif // SKYBOX_FRAG_H
