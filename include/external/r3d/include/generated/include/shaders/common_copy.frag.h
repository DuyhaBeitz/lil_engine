#ifndef COMMON_COPY_FRAG_H
#define COMMON_COPY_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char COMMON_COPY_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform sampler2D uDepthTex;out vec4 FragColor;void main(){ivec2 pixCoord=ivec2(gl_FragCoord.xy);FragColor=texelFetch(uSourceTex,pixCoord,0);gl_FragDepth=texelFetch(uDepthTex,pixCoord,0).r;}"
;

#define COMMON_COPY_FRAG_SIZE 269

#ifdef __cplusplus
}
#endif

#endif // COMMON_COPY_FRAG_H
