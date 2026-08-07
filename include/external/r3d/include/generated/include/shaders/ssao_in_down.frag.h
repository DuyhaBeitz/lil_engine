#ifndef SSAO_IN_DOWN_FRAG_H
#define SSAO_IN_DOWN_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char SSAO_IN_DOWN_FRAG[] =
    "#version 330 core\nuniform usampler2D uSelectorTex;uniform sampler2D uNormalTex;layout(location=0)out vec2 FragNormal;const ivec2 OFFSETS[4]=ivec2[4](ivec2(0,0),ivec2(1,0),ivec2(0,1),ivec2(1,1));void main(){ivec2 upCoord=2*ivec2(gl_FragCoord.xy);ivec2 pxCoord=ivec2(gl_FragCoord.xy);uint index=texelFetch(uSelectorTex,pxCoord,0).r;FragNormal=texelFetch(uNormalTex,upCoord+OFFSETS[index],0).rg;}"
;

#define SSAO_IN_DOWN_FRAG_SIZE 393

#ifdef __cplusplus
}
#endif

#endif // SSAO_IN_DOWN_FRAG_H
