#ifndef DOWN_PDSS_FRAG_H
#define DOWN_PDSS_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOWN_PDSS_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform vec2 uDestTexel;out vec4 FragColor;void main(){const vec2 o00=vec2(.176777,0.)*.5;const vec2 o01=vec2(-.225772,.206826)*.5;const vec2 o02=vec2(0.034558,-.393771)*.5;const vec2 o03=vec2(.284571,.371173)*.5;const vec2 o04=vec2(-.522223,-0.092374)*.5;const vec2 o05=vec2(.494695,-.314685)*.5;const vec2 o06=vec2(-.165466,.615525)*.5;const vec2 o07=vec2(-.315561,-.607594)*.5;const vec2 o08=vec2(.684642,.250030)*.5;const vec2 o09=vec2(-.712256,.294009)*.5;const vec2 o10=vec2(.343354,-.733729)*.5;const vec2 o11=vec2(.253730,.808932)*.5;const vec2 o12=vec2(-.764746,-.443186)*.5;const vec2 o13=vec2(.897134,-.197232)*.5;const vec2 o14=vec2(-.547507,.778772)*.5;const vec2 o15=vec2(-.126487,-.976090)*.5;vec4 c=texture(uSourceTex,vTexCoord+o00*uDestTexel);c+=texture(uSourceTex,vTexCoord+o01*uDestTexel);c+=texture(uSourceTex,vTexCoord+o02*uDestTexel);c+=texture(uSourceTex,vTexCoord+o03*uDestTexel);c+=texture(uSourceTex,vTexCoord+o04*uDestTexel);c+=texture(uSourceTex,vTexCoord+o05*uDestTexel);c+=texture(uSourceTex,vTexCoord+o06*uDestTexel);c+=texture(uSourceTex,vTexCoord+o07*uDestTexel);c+=texture(uSourceTex,vTexCoord+o08*uDestTexel);c+=texture(uSourceTex,vTexCoord+o09*uDestTexel);c+=texture(uSourceTex,vTexCoord+o10*uDestTexel);c+=texture(uSourceTex,vTexCoord+o11*uDestTexel);c+=texture(uSourceTex,vTexCoord+o12*uDestTexel);c+=texture(uSourceTex,vTexCoord+o13*uDestTexel);c+=texture(uSourceTex,vTexCoord+o14*uDestTexel);c+=texture(uSourceTex,vTexCoord+o15*uDestTexel);FragColor=c*(1./16.);}"
;

#define DOWN_PDSS_FRAG_SIZE 1581

#ifdef __cplusplus
}
#endif

#endif // DOWN_PDSS_FRAG_H
