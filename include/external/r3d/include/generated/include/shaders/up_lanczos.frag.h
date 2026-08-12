#ifndef UP_LANCZOS_FRAG_H
#define UP_LANCZOS_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char UP_LANCZOS_FRAG[] =
    "#version 330 core\n#define M_PI 3.1415926535897931\n#define M_HPI 1.5707963267948966\nnoperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform sampler2D uDepthTex;out vec4 FragColor;vec4 Lanczos(vec4 x){x+=1e-5;return sin(x*M_PI)*sin(x*M_HPI)/(x*x);}vec4 SampleLanczos(sampler2D tex,vec2 texCoord){vec2 texSize=vec2(textureSize(tex,0));vec2 texel=1./texSize;vec2 dx=vec2(1.,0.);vec2 dy=vec2(0.,1.);vec2 pc=texCoord*texSize;vec2 tc=floor(pc-.5)+.5;mat4 w;w[0]=Lanczos(vec4(1.,distance(pc,tc-dy),distance(pc,tc+dx-dy),1.));w[1]=Lanczos(vec4(distance(pc,tc-dx),distance(pc,tc),distance(pc,tc+dx),distance(pc,tc+2.*dx)));w[2]=Lanczos(vec4(distance(pc,tc-dx+dy),distance(pc,tc+dy),distance(pc,tc+dx+dy),distance(pc,tc+2.*dx+dy)));w[3]=Lanczos(vec4(1.,distance(pc,tc+2.*dy),distance(pc,tc+dx+2.*dy),1.));dx*=texel;dy*=texel;tc*=texel;vec3 c10=textureLod(tex,tc-dy,0.).rgb;vec3 c20=textureLod(tex,tc+dx-dy,0.).rgb;vec3 c01=textureLod(tex,tc-dx,0.).rgb;vec3 c11=textureLod(tex,tc,0.).rgb;vec3 c21=textureLod(tex,tc+dx,0.).rgb;vec3 c31=textureLod(tex,tc+2.*dx,0.).rgb;vec3 c02=textureLod(tex,tc-dx+dy,0.).rgb;vec3 c12=textureLod(tex,tc+dy,0.).rgb;vec3 c22=textureLod(tex,tc+dx+dy,0.).rgb;vec3 c32=textureLod(tex,tc+2.*dx+dy,0.).rgb;vec3 c13=textureLod(tex,tc+2.*dy,0.).rgb;vec3 c23=textureLod(tex,tc+dx+2.*dy,0.).rgb;vec3 color=w[0][1]*c10+w[0][2]*c20+w[1][0]*c01+w[1][1]*c11+w[1][2]*c21+w[1][3]*c31+w[2][0]*c02+w[2][1]*c12+w[2][2]*c22+w[2][3]*c32+w[3][1]*c13+w[3][2]*c23;float weightSum=0.;for(int i=0;i<4;++i)for(int j=0;j<4;++j)weightSum+=w[i][j];return vec4(color/weightSum,1.);}void main(){FragColor=SampleLanczos(uSourceTex,vTexCoord);gl_FragDepth=texture(uDepthTex,vTexCoord).r;}"
;

#define UP_LANCZOS_FRAG_SIZE 1686

#ifdef __cplusplus
}
#endif

#endif // UP_LANCZOS_FRAG_H
