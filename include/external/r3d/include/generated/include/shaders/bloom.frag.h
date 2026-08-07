#ifndef BLOOM_FRAG_H
#define BLOOM_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char BLOOM_FRAG[] =
    "#version 330 core\n#define BLOOM_MIX 1\n#define BLOOM_ADDITIVE 2\n#define BLOOM_SCREEN 3\nnoperspective in vec2 vTexCoord;uniform sampler2D uSceneTex;uniform sampler2D uBloomTex;uniform lowp int uBloomMode;uniform float uBloomIntensity;out vec3 FragColor;void main(){vec3 color=texture(uSceneTex,vTexCoord).rgb;vec3 bloom=texture(uBloomTex,vTexCoord).rgb;if(uBloomMode==BLOOM_MIX){color=mix(color,bloom,uBloomIntensity);}else if(uBloomMode==BLOOM_ADDITIVE){color+=bloom*uBloomIntensity;}else if(uBloomMode==BLOOM_SCREEN){bloom=clamp(bloom*uBloomIntensity,vec3(0.),vec3(1.));color=max((color+bloom)-(color*bloom),vec3(0.));}FragColor=vec3(color);}"
;

#define BLOOM_FRAG_SIZE 642

#ifdef __cplusplus
}
#endif

#endif // BLOOM_FRAG_H
