#ifndef DOF_BLUR_FRAG_H
#define DOF_BLUR_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOF_BLUR_FRAG[] =
    "#version 330 core\n#define M_PI 3.1415926535897931\n#define M_HPI 1.5707963267948966\n#define M_TAU 6.2831853071795862\n#define M_INV_PI .3183098861837907\n#define M_PHI 1.6180339887498949\n#define M_PHI_FRAC .6180339887498949\n#define M_GOLDEN_ANGLE 2.3999632297286535\nvec3 M_Rotate3D(vec3 v,vec4 q){vec3 t=2.*cross(q.xyz,v);return v+q.w*t+cross(q.xyz,t);}mat3 M_OrthonormalBasis(vec3 n){float sgn=n.z>=0.?1.:-1.;float a=-1./(sgn+n.z);float b=n.x*n.y*a;vec3 t=vec3(1.+sgn*n.x*n.x*a,sgn*b,-sgn*n.x);vec3 bt=vec3(b,sgn+n.y*n.y*a,-n.y);return mat3(t,bt,n);}vec2 M_OctahedronWrap(vec2 val){return(1.-abs(val.yx))*mix(vec2(-1.),vec2(1.),vec2(greaterThanEqual(val.xy,vec2(0.))));}vec3 M_DecodeOctahedral(vec2 encoded){encoded=encoded*2.-1.;vec3 normal;normal.z=1.-abs(encoded.x)-abs(encoded.y);normal.xy=normal.z>=0.?encoded.xy:M_OctahedronWrap(encoded.xy);return normalize(normal);}vec2 M_EncodeOctahedral(vec3 normal){normal/=abs(normal.x)+abs(normal.y)+abs(normal.z);normal.xy=normal.z>=0.?normal.xy:M_OctahedronWrap(normal.xy);normal.xy=normal.xy*.5+.5;return normal.xy;}vec3 M_NormalScale(vec3 normal,float scale){normal.xy*=scale;normal.z=sqrt(1.-clamp(dot(normal.xy,normal.xy),0.,1.));return normal;}float M_HashIGN(vec2 pos){const vec3 magic=vec3(0.06711056,0.00583715,52.9829189);return fract(magic.z*fract(dot(pos,magic.xy)));}float M_HashIGN(vec2 pos,float frame){vec3 magic=vec3(0.06711056,0.00583715,52.9829189);return fract(magic.z*fract(dot(vec3(pos,frame),magic)));}float M_HashR2(vec2 p){const vec2 k=vec2(.75487766624669276,.56984029099805327);return fract(dot(p,k));}noperspective in vec2 vTexCoord;uniform sampler2D uSceneTex;uniform sampler2D uDepthTex;uniform float uMaxBlurSize;const float RAD_SCALE=.5;out vec4 FragColor;void main(){vec2 texelSize=1./vec2(textureSize(uSceneTex,0));vec4 centerScene=texture(uSceneTex,vTexCoord);float centerDepth=texture(uDepthTex,vTexCoord).r;float centerSize=abs(centerScene.a)*uMaxBlurSize;vec3 color=centerScene.rgb;float radius=RAD_SCALE;float tot=1.;for(float ang=0.;radius<uMaxBlurSize;ang+=M_GOLDEN_ANGLE){vec2 uv=vTexCoord+vec2(cos(ang),sin(ang))*texelSize*radius;vec4 sampleScene=texture(uSceneTex,uv);float sampleDepth=texture(uDepthTex,uv).r;float sampleSize=abs(sampleScene.a)*uMaxBlurSize;if(sampleDepth>centerDepth){sampleSize=clamp(sampleSize,0.,centerSize*2.);}float w=smoothstep(radius-.5,radius+.5,sampleSize);color+=mix(color/tot,sampleScene.rgb,w);radius+=RAD_SCALE/radius;tot+=1.;}float w=exp(-.5*centerSize*centerSize);FragColor=vec4(color/tot,clamp(w,0.,1.));}"
;

#define DOF_BLUR_FRAG_SIZE 2529

#ifdef __cplusplus
}
#endif

#endif // DOF_BLUR_FRAG_H
