#ifndef DOWN_RGSS_FRAG_H
#define DOWN_RGSS_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOWN_RGSS_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform sampler2D uDepthTex;uniform vec2 uDestTexel;out vec4 FragColor;vec4 SampleColor(sampler2D tex,vec2 texCoord,vec2 dstTexel){const vec2 o0=vec2(-3.,1.)/8.;const vec2 o1=vec2(1.,3.)/8.;const vec2 o2=vec2(3.,-1.)/8.;const vec2 o3=vec2(-1.,-3.)/8.;vec4 c0=texture(tex,texCoord+o0*dstTexel);vec4 c1=texture(tex,texCoord+o1*dstTexel);vec4 c2=texture(tex,texCoord+o2*dstTexel);vec4 c3=texture(tex,texCoord+o3*dstTexel);return(c0+c1+c2+c3)*.25;}void main(){FragColor=SampleColor(uSourceTex,vTexCoord,uDestTexel);gl_FragDepth=texture(uDepthTex,vTexCoord).r;}"
;

#define DOWN_RGSS_FRAG_SIZE 635

#ifdef __cplusplus
}
#endif

#endif // DOWN_RGSS_FRAG_H
