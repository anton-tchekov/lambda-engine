#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "vector.h"
#include "opengl.h"

int skybox_init(void);
void skybox_destroy(void);
void skybox_bind_cubemap(GLuint cubemap);
void skybox_render(mat4 perspective);

#endif

