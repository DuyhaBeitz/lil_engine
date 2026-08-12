#ifndef DOWN_PDSS_FRAG_H
#define DOWN_PDSS_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOWN_PDSS_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform sampler2D uDepthTex;uniform vec2 uDestTexel;out vec4 FragColor;vec4 SampleColor(sampler2D tex,vec2 texCoord,vec2 dstTexel){const vec2 o00=vec2(.176777,0.)*.5;const vec2 o01=vec2(-.225772,.206826)*.5;const vec2 o02=vec2(0.034558,-.393771)*.5;const vec2 o03=vec2(.284571,.371173)*.5;const vec2 o04=vec2(-.522223,-0.092374)*.5;const vec2 o05=vec2(.494695,-.314685)*.5;const vec2 o06=vec2(-.165466,.615525)*.5;const vec2 o07=vec2(-.315561,-.607594)*.5;const vec2 o08=vec2(.684642,.250030)*.5;const vec2 o09=vec2(-.712256,.294009)*.5;const vec2 o10=vec2(.343354,-.733729)*.5;const vec2 o11=vec2(.253730,.808932)*.5;const vec2 o12=vec2(-.764746,-.443186)*.5;const vec2 o13=vec2(.897134,-.197232)*.5;const vec2 o14=vec2(-.547507,.778772)*.5;const vec2 o15=vec2(-.126487,-.976090)*.5;vec4 c=texture(tex,texCoord+o00*dstTexel);c+=texture(tex,texCoord+o01*dstTexel);c+=texture(tex,texCoord+o02*dstTexel);c+=texture(tex,texCoord+o03*dstTexel);c+=texture(tex,texCoord+o04*dstTexel);c+=texture(tex,texCoord+o05*dstTexel);c+=texture(tex,texCoord+o06*dstTexel);c+=texture(tex,texCoord+o07*dstTexel);c+=texture(tex,texCoord+o08*dstTexel);c+=texture(tex,texCoord+o09*dstTexel);c+=texture(tex,texCoord+o10*dstTexel);c+=texture(tex,texCoord+o11*dstTexel);c+=texture(tex,texCoord+o12*dstTexel);c+=texture(tex,texCoord+o13*dstTexel);c+=texture(tex,texCoord+o14*dstTexel);c+=texture(tex,texCoord+o15*dstTexel);return c*(1./16.);}void main(){FragColor=SampleColor(uSourceTex,vTexCoord,uDestTexel);gl_FragDepth=texture(uDepthTex,vTexCoord).r;}"
;

#define DOWN_PDSS_FRAG_SIZE 1606

#ifdef __cplusplus
}
#endif

#endif // DOWN_PDSS_FRAG_H
