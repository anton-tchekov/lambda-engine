#ifndef __AABB__
#define __AABB__

#include "vector.h"

typedef struct AABB
{
	float VelocityY;
	vec3 Min, Max;
} AABB;

void aabb_normalize(AABB *a);
void aabb_print(AABB *a);
void aabb_enclose(AABB *out, AABB *in, int count);
void aabb_transform(AABB *out, AABB *in, mat4 model);
int aabb_intersect(AABB *a, AABB *b);
void aabb_instance(AABB *out, AABB *in, mat4 model);

#endif

