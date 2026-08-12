#ifndef SKYBOX_VERT_H
#define SKYBOX_VERT_H

#ifdef __cplusplus
extern "C" {
#endif

static const char SKYBOX_VERT[] =
    "#version 330 core\nconst vec2 positions[3]=vec2[](vec2(-1.,-1.),vec2(3.,-1.),vec2(-1.,3.));out vec3 vViewRay;uniform mat4 uMatInvProj;uniform mat4 uMatInvView;void main(){vec2 pos=positions[gl_VertexID];gl_Position=vec4(pos,1.,1.);vec4 unprojected=uMatInvProj*vec4(pos,1.,1.);vViewRay=mat3(uMatInvView)*unprojected.xyz;}"
;

#define SKYBOX_VERT_SIZE 319

#ifdef __cplusplus
}
#endif

#endif // SKYBOX_VERT_H
