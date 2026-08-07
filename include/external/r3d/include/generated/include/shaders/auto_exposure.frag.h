#ifndef AUTO_EXPOSURE_FRAG_H
#define AUTO_EXPOSURE_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char AUTO_EXPOSURE_FRAG[] =
    "#version 330 core\nuniform sampler2D uSceneTex;uniform sampler2D uExposureTex;out vec4 FragColor;void main(){vec4 color=texelFetch(uSceneTex,ivec2(gl_FragCoord.xy),0);float exposure=texelFetch(uExposureTex,ivec2(0),0).r;FragColor=vec4(color.rgb*exposure,color.a);}"
;

#define AUTO_EXPOSURE_FRAG_SIZE 263

#ifdef __cplusplus
}
#endif

#endif // AUTO_EXPOSURE_FRAG_H
