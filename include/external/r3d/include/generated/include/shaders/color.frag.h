#ifndef COLOR_FRAG_H
#define COLOR_FRAG_H

#ifdef __cplusplus
extern "C" {
#endif

static const char COLOR_FRAG[] =
    "#version 330 core\nuniform vec4 uColor;layout(location=0)out vec4 FragColor;void main(){FragColor=uColor;}"
;

#define COLOR_FRAG_SIZE 105

#ifdef __cplusplus
}
#endif

#endif // COLOR_FRAG_H
