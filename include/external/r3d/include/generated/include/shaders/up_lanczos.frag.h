#ifndef UP_LANCZOS_FRAG_H
#define UP_LANCZOS_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char UP_LANCZOS_FRAG[] =
    "#version 330 core\n#define M_PI 3.1415926535897931\n#define M_HPI 1.5707963267948966\n#define M_TAU 6.2831853071795862\n#define M_INV_PI .3183098861837907\n#define M_PHI 1.6180339887498949\n#define M_PHI_FRAC .6180339887498949\n#define M_GOLDEN_ANGLE 2.3999632297286535\nvec3 M_Rotate3D(vec3 v,vec4 q){vec3 t=2.*cross(q.xyz,v);return v+q.w*t+cross(q.xyz,t);}mat3 M_OrthonormalBasis(vec3 n){float sgn=n.z>=0.?1.:-1.;float a=-1./(sgn+n.z);float b=n.x*n.y*a;vec3 t=vec3(1.+sgn*n.x*n.x*a,sgn*b,-sgn*n.x);vec3 bt=vec3(b,sgn+n.y*n.y*a,-n.y);return mat3(t,bt,n);}vec2 M_OctahedronWrap(vec2 val){return(1.-abs(val.yx))*mix(vec2(-1.),vec2(1.),vec2(greaterThanEqual(val.xy,vec2(0.))));}vec3 M_DecodeOctahedral(vec2 encoded){encoded=encoded*2.-1.;vec3 normal;normal.z=1.-abs(encoded.x)-abs(encoded.y);normal.xy=normal.z>=0.?encoded.xy:M_OctahedronWrap(encoded.xy);return normalize(normal);}vec2 M_EncodeOctahedral(vec3 normal){normal/=abs(normal.x)+abs(normal.y)+abs(normal.z);normal.xy=normal.z>=0.?normal.xy:M_OctahedronWrap(normal.xy);normal.xy=normal.xy*.5+.5;return normal.xy;}vec3 M_NormalScale(vec3 normal,float scale){normal.xy*=scale;normal.z=sqrt(1.-clamp(dot(normal.xy,normal.xy),0.,1.));return normal;}float M_HashIGN(vec2 pos){const vec3 magic=vec3(0.06711056,0.00583715,52.9829189);return fract(magic.z*fract(dot(pos,magic.xy)));}float M_HashIGN(vec2 pos,float frame){vec3 magic=vec3(0.06711056,0.00583715,52.9829189);return fract(magic.z*fract(dot(vec3(pos,frame),magic)));}float M_HashR2(vec2 p){const vec2 k=vec2(.75487766624669276,.56984029099805327);return fract(dot(p,k));}noperspective in vec2 vTexCoord;uniform sampler2D uSourceTex;uniform vec2 uSourceTexel;out vec4 FragColor;vec4 Lanczos(vec4 x){return(x==vec4(0.))?vec4(M_PI*M_HPI):sin(x*M_HPI)*sin(x*M_PI)/(x*x);}void main(){vec3 color;mat4 weights;vec2 dx=vec2(1.,0.);vec2 dy=vec2(0.,1.);vec2 pc=vTexCoord*vec2(textureSize(uSourceTex,0));vec2 tc=floor(pc-vec2(.5,.5))+vec2(.5,.5);weights[0]=Lanczos(vec4(1.,distance(pc,tc-dy),distance(pc,tc+dx-dy),1.));weights[1]=Lanczos(vec4(distance(pc,tc-dx),distance(pc,tc),distance(pc,tc+dx),distance(pc,tc+2.*dx)));weights[2]=Lanczos(vec4(distance(pc,tc-dx+dy),distance(pc,tc+dy),distance(pc,tc+dx+dy),distance(pc,tc+2.*dx+dy)));weights[3]=Lanczos(vec4(1.,distance(pc,tc+2.*dy),distance(pc,tc+dx+2.*dy),1.));dx=dx*uSourceTexel;dy=dy*uSourceTexel;tc=tc*uSourceTexel;vec3 c10=texture(uSourceTex,tc-dy).rgb;vec3 c20=texture(uSourceTex,tc+dx-dy).rgb;vec3 c01=texture(uSourceTex,tc-dx).rgb;vec3 c11=texture(uSourceTex,tc).rgb;vec3 c21=texture(uSourceTex,tc+dx).rgb;vec3 c31=texture(uSourceTex,tc+2.*dx).rgb;vec3 c02=texture(uSourceTex,tc-dx+dy).rgb;vec3 c12=texture(uSourceTex,tc+dy).rgb;vec3 c22=texture(uSourceTex,tc+dx+dy).rgb;vec3 c32=texture(uSourceTex,tc+2.*dx+dy).rgb;vec3 c13=texture(uSourceTex,tc+2.*dy).rgb;vec3 c23=texture(uSourceTex,tc+dx+2.*dy).rgb;vec3 o=vec3(0.);color=weights[0][0]*o+weights[0][1]*c10+weights[0][2]*c20+weights[0][3]*o;color+=weights[1][0]*c01+weights[1][1]*c11+weights[1][2]*c21+weights[1][3]*c31;color+=weights[2][0]*c02+weights[2][1]*c12+weights[2][2]*c22+weights[2][3]*c32;color+=weights[3][0]*o+weights[3][1]*c13+weights[3][2]*c23+weights[3][3]*o;float weightSum=0.;for(int i=0;i<4;i++){for(int j=0;j<4;j++){weightSum+=weights[i][j];}}FragColor=vec4(color/weightSum,1.);}"
;

#define UP_LANCZOS_FRAG_SIZE 3302

#ifdef __cplusplus
}
#endif

#endif // UP_LANCZOS_FRAG_H
