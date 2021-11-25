#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__

#include "vector.h"
#include "physics.h"

int wireframe_init(void);
void wireframe_use(mat4 perspective);
void wireframe_aabb(AABB *box);
void wireframe_render(const vec4 color);
void wireframe_destroy(void);

#endif

