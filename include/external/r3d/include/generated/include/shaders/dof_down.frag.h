#ifndef DOF_DOWN_FRAG_H
#define DOF_DOWN_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOF_DOWN_FRAG[] =
    "#version 330 core\nuniform sampler2D uSceneTex;uniform sampler2D uCoCTex;out vec4 FragCoC;void main(){ivec2 pixCoord=2*ivec2(gl_FragCoord.xy);vec4 color0=texelFetch(uSceneTex,pixCoord+ivec2(0,0),0);vec4 color1=texelFetch(uSceneTex,pixCoord+ivec2(1,0),0);vec4 color2=texelFetch(uSceneTex,pixCoord+ivec2(0,1),0);vec4 color3=texelFetch(uSceneTex,pixCoord+ivec2(1,1),0);float coc0=texelFetch(uCoCTex,pixCoord+ivec2(0,0),0).r;float coc1=texelFetch(uCoCTex,pixCoord+ivec2(1,0),0).r;float coc2=texelFetch(uCoCTex,pixCoord+ivec2(0,1),0).r;float coc3=texelFetch(uCoCTex,pixCoord+ivec2(1,1),0).r;vec4 color=(color0+color1+color2+color3)*.25;float coc=min(min(min(coc0,coc1),coc2),coc3);FragCoC=vec4(color.rgb,coc);}"
;

#define DOF_DOWN_FRAG_SIZE 704

#ifdef __cplusplus
}
#endif

#endif // DOF_DOWN_FRAG_H
