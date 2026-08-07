#ifndef SSR_IN_DOWN_FRAG_H
#define SSR_IN_DOWN_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char SSR_IN_DOWN_FRAG[] =
    "#version 330 core\nuniform usampler2D uSelectorTex;uniform sampler2D uDiffuseTex;uniform sampler2D uSpecularTex;uniform sampler2D uNormalTex;layout(location=0)out vec3 FragDiffuse;layout(location=1)out vec3 FragSpecular;layout(location=2)out vec2 FragNormal;const ivec2 OFFSETS[4]=ivec2[4](ivec2(0,0),ivec2(1,0),ivec2(0,1),ivec2(1,1));void main(){ivec2 upCoord=2*ivec2(gl_FragCoord.xy);ivec2 pxCoord=ivec2(gl_FragCoord.xy);uint index=texelFetch(uSelectorTex,pxCoord,0).r;FragDiffuse=texelFetch(uDiffuseTex,upCoord+OFFSETS[index],0).rgb;FragSpecular=texelFetch(uSpecularTex,upCoord+OFFSETS[index],0).rgb;FragNormal=texelFetch(uNormalTex,upCoord+OFFSETS[index],0).rg;}"
;

#define SSR_IN_DOWN_FRAG_SIZE 665

#ifdef __cplusplus
}
#endif

#endif // SSR_IN_DOWN_FRAG_H
