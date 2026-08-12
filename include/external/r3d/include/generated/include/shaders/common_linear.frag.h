#ifndef COMMON_LINEAR_FRAG_H
#define COMMON_LINEAR_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char COMMON_LINEAR_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform sampler2D uDepthTex;out vec4 FragColor;void main(){FragColor=texture(uSourceTex,vTexCoord);gl_FragDepth=texture(uDepthTex,vTexCoord).r;}"
;

#define COMMON_LINEAR_FRAG_SIZE 223

#ifdef __cplusplus
}
#endif

#endif // COMMON_LINEAR_FRAG_H
