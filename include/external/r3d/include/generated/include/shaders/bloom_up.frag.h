#ifndef BLOOM_UP_FRAG_H
#define BLOOM_UP_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char BLOOM_UP_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;layout(location=0)out vec3 FragColor;uniform sampler2D uTexture;uniform vec2 uFilterRadius;uniform float uSrcLevel;void main(){float x=uFilterRadius.x;float y=uFilterRadius.y;vec3 a=textureLod(uTexture,vec2(vTexCoord.x-x,vTexCoord.y+y),uSrcLevel).rgb;vec3 b=textureLod(uTexture,vec2(vTexCoord.x,vTexCoord.y+y),uSrcLevel).rgb;vec3 c=textureLod(uTexture,vec2(vTexCoord.x+x,vTexCoord.y+y),uSrcLevel).rgb;vec3 d=textureLod(uTexture,vec2(vTexCoord.x-x,vTexCoord.y),uSrcLevel).rgb;vec3 e=textureLod(uTexture,vec2(vTexCoord.x,vTexCoord.y),uSrcLevel).rgb;vec3 f=textureLod(uTexture,vec2(vTexCoord.x+x,vTexCoord.y),uSrcLevel).rgb;vec3 g=textureLod(uTexture,vec2(vTexCoord.x-x,vTexCoord.y-y),uSrcLevel).rgb;vec3 h=textureLod(uTexture,vec2(vTexCoord.x,vTexCoord.y-y),uSrcLevel).rgb;vec3 i=textureLod(uTexture,vec2(vTexCoord.x+x,vTexCoord.y-y),uSrcLevel).rgb;FragColor=e*4.;FragColor+=(b+d+f+h)*2.;FragColor+=(a+c+g+i);FragColor*=1./16.;}"
;

#define BLOOM_UP_FRAG_SIZE 976

#ifdef __cplusplus
}
#endif

#endif // BLOOM_UP_FRAG_H
