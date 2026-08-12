#ifndef COMMON_NEAREST_FRAG_H
#define COMMON_NEAREST_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char COMMON_NEAREST_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform sampler2D uDepthTex;out vec4 FragColor;void main(){ivec2 sourceSize=textureSize(uSourceTex,0);ivec2 pixCoord=ivec2(vTexCoord*vec2(sourceSize));pixCoord=clamp(pixCoord,ivec2(0),sourceSize-1);FragColor=texelFetch(uSourceTex,pixCoord,0);gl_FragDepth=texelFetch(uDepthTex,pixCoord,0).r;}"
;

#define COMMON_NEAREST_FRAG_SIZE 370

#ifdef __cplusplus
}
#endif

#endif // COMMON_NEAREST_FRAG_H
