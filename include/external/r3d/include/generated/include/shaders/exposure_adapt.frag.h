#ifndef EXPOSURE_ADAPT_FRAG_H
#define EXPOSURE_ADAPT_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char EXPOSURE_ADAPT_FRAG[] =
    "#version 330 core\nuniform sampler2D uMeasuredLogLumTex;uniform sampler2D uPrevAutoExposureTex;uniform float uDeltaTime;uniform float uMinLogLum;uniform float uMaxLogLum;uniform float uSpeedUp;uniform float uSpeedDown;uniform float uExposureCompLog;out vec4 FragColor;void main(){const float LOG_MIDDLE_GRAY=-1.7147984280919266;float measuredLogLum=texelFetch(uMeasuredLogLumTex,ivec2(0,0),0).r;float prevLogLum=texelFetch(uPrevAutoExposureTex,ivec2(0,0),0).g;measuredLogLum=clamp(measuredLogLum,uMinLogLum,uMaxLogLum);float speed=measuredLogLum>prevLogLum?uSpeedUp:uSpeedDown;float blend=1.-exp(-uDeltaTime*speed);float adaptedLogLum=mix(prevLogLum,measuredLogLum,blend);float exposure=exp(LOG_MIDDLE_GRAY-adaptedLogLum+uExposureCompLog);FragColor=vec4(exposure,adaptedLogLum,0.,1.);}"
;

#define EXPOSURE_ADAPT_FRAG_SIZE 784

#ifdef __cplusplus
}
#endif

#endif // EXPOSURE_ADAPT_FRAG_H
