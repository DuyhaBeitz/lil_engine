#ifndef BLUR_DOWN_FRAG_H
#define BLUR_DOWN_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char BLUR_DOWN_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform int uSourceLod;out vec4 FragColor;void main(){vec2 halfPixel=.5/vec2(textureSize(uSourceTex,uSourceLod));float lod=float(uSourceLod);vec4 sum=textureLod(uSourceTex,vTexCoord,lod)*4.;sum+=textureLod(uSourceTex,vTexCoord-halfPixel.xy,lod);sum+=textureLod(uSourceTex,vTexCoord+halfPixel.xy,lod);sum+=textureLod(uSourceTex,vTexCoord+vec2(halfPixel.x,-halfPixel.y),lod);sum+=textureLod(uSourceTex,vTexCoord-vec2(halfPixel.x,-halfPixel.y),lod);FragColor=sum/8.;}"
;

#define BLUR_DOWN_FRAG_SIZE 543

#ifdef __cplusplus
}
#endif

#endif // BLUR_DOWN_FRAG_H
