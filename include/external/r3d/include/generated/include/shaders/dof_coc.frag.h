#ifndef DOF_COC_FRAG_H
#define DOF_COC_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOF_COC_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uDepthTex;uniform float uFocusPoint;uniform float uFocusScale;uniform float uNearScale;out float FragCoC;void main(){float depth=texelFetch(uDepthTex,ivec2(gl_FragCoord.xy),0).r;float coc=(1./uFocusPoint-1./depth)*uFocusScale;coc*=mix(uNearScale,1.,step(0.,coc));FragCoC=clamp(coc,-1.,1.);}"
;

#define DOF_COC_FRAG_SIZE 358

#ifdef __cplusplus
}
#endif

#endif // DOF_COC_FRAG_H
