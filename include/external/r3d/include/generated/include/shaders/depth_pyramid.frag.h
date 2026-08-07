#ifndef DEPTH_PYRAMID_FRAG_H
#define DEPTH_PYRAMID_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DEPTH_PYRAMID_FRAG[] =
    "#version 330 core\nuniform sampler2D uDepthTex;layout(location=0)out float FragDepth;layout(location=1)out uint FragSelect;void main(){ivec2 pixCoord=2*ivec2(gl_FragCoord.xy);ivec2 p0=pixCoord+ivec2(0,0);ivec2 p1=pixCoord+ivec2(1,0);ivec2 p2=pixCoord+ivec2(0,1);ivec2 p3=pixCoord+ivec2(1,1);float d0=texelFetch(uDepthTex,p0,0).r;float d1=texelFetch(uDepthTex,p1,0).r;float d2=texelFetch(uDepthTex,p2,0).r;float d3=texelFetch(uDepthTex,p3,0).r;FragDepth=d0,FragSelect=0u;if(d1<FragDepth){FragDepth=d1;FragSelect=1u;}if(d2<FragDepth){FragDepth=d2;FragSelect=2u;}if(d3<FragDepth){FragDepth=d3;FragSelect=3u;}}"
;

#define DEPTH_PYRAMID_FRAG_SIZE 605

#ifdef __cplusplus
}
#endif

#endif // DEPTH_PYRAMID_FRAG_H
