#ifndef CUBEMAP_VERT_H
#define CUBEMAP_VERT_H

#ifdef __cplusplus
extern "C" {
#endif

static const char CUBEMAP_VERT[] =
    "#version 330 core\nlayout(location=0)in vec3 aPosition;layout(location=1)in vec2 aTexCoord;uniform mat4 uMatProj;uniform mat4 uMatView;out vec3 vPosition;out vec2 vTexCoord;void main(){vPosition=aPosition;vTexCoord=aTexCoord;gl_Position=uMatProj*uMatView*vec4(vPosition,1.);}"
;

#define CUBEMAP_VERT_SIZE 274

#ifdef __cplusplus
}
#endif

#endif // CUBEMAP_VERT_H
