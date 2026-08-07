#ifndef DOWN_RGSS_FRAG_H
#define DOWN_RGSS_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOWN_RGSS_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform vec2 uDestTexel;out vec4 FragColor;void main(){const vec2 o0=vec2(-3.,1.)/8.;const vec2 o1=vec2(1.,3.)/8.;const vec2 o2=vec2(3.,-1.)/8.;const vec2 o3=vec2(-1.,-3.)/8.;vec4 c0=texture(uSourceTex,vTexCoord+o0*uDestTexel);vec4 c1=texture(uSourceTex,vTexCoord+o1*uDestTexel);vec4 c2=texture(uSourceTex,vTexCoord+o2*uDestTexel);vec4 c3=texture(uSourceTex,vTexCoord+o3*uDestTexel);FragColor=(c0+c1+c2+c3)*.25;}"
;

#define DOWN_RGSS_FRAG_SIZE 491

#ifdef __cplusplus
}
#endif

#endif // DOWN_RGSS_FRAG_H
