#ifndef DOF_FRAG_H
#define DOF_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char DOF_FRAG[] =
    "#version 330 core\nnoperspective in vec2 vTexCoord;uniform sampler2D uSceneTex;uniform sampler2D uBlurTex;out vec4 FragColor;void main(){vec4 sharp=texelFetch(uSceneTex,ivec2(gl_FragCoord),0);vec4 blur=texture(uBlurTex,vTexCoord);FragColor=vec4(mix(blur.rgb,sharp.rgb,blur.a),1.);}"
;

#define DOF_FRAG_SIZE 280

#ifdef __cplusplus
}
#endif

#endif // DOF_FRAG_H
