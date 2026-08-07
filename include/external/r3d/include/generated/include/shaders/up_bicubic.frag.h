#ifndef UP_BICUBIC_FRAG_H
#define UP_BICUBIC_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char UP_BICUBIC_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform vec2 uSourceTexel;out vec4 FragColor;void main(){vec2 samplePos=vTexCoord*vec2(textureSize(uSourceTex,0));vec2 texPos1=floor(samplePos-.5)+.5;vec2 f=samplePos-texPos1;vec2 w0=f*(-.5+f*(1.-.5*f));vec2 w1=1.+f*f*(-2.5+1.5*f);vec2 w2=f*(.5+f*(2.-1.5*f));vec2 w3=f*f*(-.5+.5*f);vec2 w12=w1+w2;vec2 offset12=w2/(w1+w2);vec2 texPos0=texPos1-1.;vec2 texPos3=texPos1+2.;vec2 texPos12=texPos1+offset12;texPos0*=uSourceTexel;texPos3*=uSourceTexel;texPos12*=uSourceTexel;vec4 result=vec4(0.);result+=texture(uSourceTex,vec2(texPos0.x,texPos0.y))*w0.x*w0.y;result+=texture(uSourceTex,vec2(texPos12.x,texPos0.y))*w12.x*w0.y;result+=texture(uSourceTex,vec2(texPos3.x,texPos0.y))*w3.x*w0.y;result+=texture(uSourceTex,vec2(texPos0.x,texPos12.y))*w0.x*w12.y;result+=texture(uSourceTex,vec2(texPos12.x,texPos12.y))*w12.x*w12.y;result+=texture(uSourceTex,vec2(texPos3.x,texPos12.y))*w3.x*w12.y;result+=texture(uSourceTex,vec2(texPos0.x,texPos3.y))*w0.x*w3.y;result+=texture(uSourceTex,vec2(texPos12.x,texPos3.y))*w12.x*w3.y;result+=texture(uSourceTex,vec2(texPos3.x,texPos3.y))*w3.x*w3.y;FragColor=result;}"
;

#define UP_BICUBIC_FRAG_SIZE 1174

#ifdef __cplusplus
}
#endif

#endif // UP_BICUBIC_FRAG_H
