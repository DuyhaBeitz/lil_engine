#ifndef BLUR_UP_FRAG_H
#define BLUR_UP_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char BLUR_UP_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform int uSourceLod;out vec4 FragColor;void main(){vec2 halfPixel=.5/vec2(textureSize(uSourceTex,uSourceLod));float lod=float(uSourceLod);vec4 sum=textureLod(uSourceTex,vTexCoord+vec2(-halfPixel.x*2.,0.),lod);sum+=textureLod(uSourceTex,vTexCoord+vec2(-halfPixel.x,halfPixel.y),lod)*2.;sum+=textureLod(uSourceTex,vTexCoord+vec2(0.,halfPixel.y*2.),lod);sum+=textureLod(uSourceTex,vTexCoord+vec2(halfPixel.x,halfPixel.y),lod)*2.;sum+=textureLod(uSourceTex,vTexCoord+vec2(halfPixel.x*2.,0.),lod);sum+=textureLod(uSourceTex,vTexCoord+vec2(halfPixel.x,-halfPixel.y),lod)*2.;sum+=textureLod(uSourceTex,vTexCoord+vec2(0.,-halfPixel.y*2.),lod);sum+=textureLod(uSourceTex,vTexCoord+vec2(-halfPixel.x,-halfPixel.y),lod)*2.;FragColor=sum/12.;}"
;

#define BLUR_UP_FRAG_SIZE 813

#ifdef __cplusplus
}
#endif

#endif // BLUR_UP_FRAG_H
