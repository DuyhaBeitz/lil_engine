#ifndef DEPTH_FRAG_H
#define DEPTH_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DEPTH_FRAG[] =
    "#version 330 core\nstruct Frame{vec2 screenSize;vec2 texelSize;float time;int index;};layout(std140)uniform FrameBlock{Frame uFrame;};smooth in vec3 vPosition;smooth in vec2 vTexCoord;smooth in vec4 vColor;uniform sampler2D uAlbedoMap;\nuniform float uAlphaCutoff;\n#define POSITION vPosition\nvec2 TEXCOORD=vec2(0.);vec3 TANGENT=vec3(0.);vec3 BITANGENT=vec3(0.);vec3 NORMAL=vec3(0.);vec3 ALBEDO=vec3(0.);float ALPHA=0.;vec3 EMISSION=vec3(0.);vec3 NORMAL_MAP=vec3(0.);float OCCLUSION=0.;float ROUGHNESS=0.;float METALNESS=0.;float SPECULAR=0.;int FRAME_INDEX=0;float TIME=0.;vec4 SampleAlbedo(vec2 texCoord){return vColor*texture(uAlbedoMap,texCoord);}vec3 SampleEmission(vec2 texCoord){\n vec3 emission=vec3(0.);\n#if !defined(UNLIT) && !defined(PROBE_UNLIT) && !defined(DEPTH) && !defined(DEPTH_CUBE)\n emission=vEmission*texture(uEmissionMap,texCoord).rgb;\n#endif\n return emission;}vec3 SampleNormal(vec2 texCoord){\n vec3 normal=vec3(0.);\n#if !defined(UNLIT) && !defined(PROBE_UNLIT) && !defined(DEPTH) && !defined(DEPTH_CUBE)\n normal=texture(uNormalMap,texCoord).rgb;\n#endif\n return normal;}vec4 SampleOrm(vec2 texCoord){\n vec4 ORM=vec4(0.);\n#if !defined(UNLIT) && !defined(PROBE_UNLIT) && !defined(DEPTH) && !defined(DEPTH_CUBE)\n ORM=texture(uOrmMap,texCoord);ORM.x*=uOcclusion;ORM.y*=uRoughness;ORM.z*=uMetalness;\n ORM.w=uSpecular;\n#endif\n return ORM;}void FetchMaterial(vec2 texCoord){vec4 color=vColor*texture(uAlbedoMap,texCoord);ALBEDO=color.rgb;\n ALPHA=color.a;\n#if !defined(UNLIT) && !defined(PROBE_UNLIT) && !defined(DEPTH) && !defined(DEPTH_CUBE)\n EMISSION=vEmission*texture(uEmissionMap,texCoord).rgb;NORMAL_MAP=texture(uNormalMap,texCoord).rgb;vec3 ORM=texture(uOrmMap,texCoord).rgb;OCCLUSION=uOcclusion*ORM.x;ROUGHNESS=uRoughness*ORM.y;METALNESS=uMetalness*ORM.z;\n SPECULAR=uSpecular;\n#endif\n}\n#define fragment()\nvoid SceneFragment(vec2 texCoord,mat3 tbn,float alphaCutoff){TEXCOORD=texCoord;TANGENT=tbn[0];BITANGENT=tbn[1];\n NORMAL=tbn[2];\n#if !defined(R3D_NO_AUTO_FETCH)\n vec4 color=vColor*texture(uAlbedoMap,texCoord);if(color.a<alphaCutoff)discard;ALBEDO=color.rgb;\n ALPHA=color.a;\n#if !defined(UNLIT) && !defined(PROBE_UNLIT) && !defined(DEPTH) && !defined(DEPTH_CUBE)\n EMISSION=vEmission*texture(uEmissionMap,texCoord).rgb;NORMAL_MAP=texture(uNormalMap,texCoord).rgb;vec3 ORM=texture(uOrmMap,texCoord).rgb;OCCLUSION=uOcclusion*ORM.x;ROUGHNESS=uRoughness*ORM.y;METALNESS=uMetalness*ORM.z;\n SPECULAR=uSpecular;\n#endif \n#endif \n FRAME_INDEX=uFrame.index;TIME=uFrame.time;fragment();if(ALPHA<alphaCutoff)discard;}void main(){SceneFragment(vTexCoord,mat3(1.),uAlphaCutoff);}"
;

#define DEPTH_FRAG_SIZE 2585

#ifdef __cplusplus
}
#endif

#endif // DEPTH_FRAG_H
