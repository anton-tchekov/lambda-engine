#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>
#include "types.h"

#define EPSILON 0.00000001f
#define PI      3.14159265358979323846

#define X       0
#define Y       1
#define Z       2
#define W       3

#define RAD(x)  ((x) * (PI / 180.0))
#define DEG(x)  ((x) * (180.0 / PI))

#define RADF(x)  (float)((x) * ((float)PI / 180.0f))
#define DEGF(x)  (float)((x) * (180.0f / (float)PI))

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec4 mat4[4];

i32 abs(i32 a);

/* vector */
void vec2_add(vec2 r, const vec2 a, const vec2 b);
void vec3_add(vec3 r, const vec3 a, const vec3 b);
void vec4_add(vec4 r, const vec4 a, const vec4 b);

void vec2_sub(vec2 r, const vec2 a, const vec2 b);
void vec3_sub(vec3 r, const vec3 a, const vec3 b);
void vec4_sub(vec4 r, const vec4 a, const vec4 b);

void vec2_scale(vec2 r, const vec2 v, float s);
void vec3_scale(vec3 r, const vec3 v, float s);
void vec4_scale(vec4 r, const vec4 v, float s);

void vec2_dup(vec2 r, const vec2 v);
void vec3_dup(vec3 r, const vec3 v);
void vec4_dup(vec4 r, const vec4 v);

float vec2_dot(const vec2 a, const vec2 b);
float vec3_dot(const vec3 a, const vec3 b);
float vec4_dot(const vec4 a, const vec4 b);

float vec2_len(const vec2 v);
float vec3_len(const vec3 v);
float vec4_len(const vec4 v);

void vec2_norm(vec2 r, const vec2 v);
void vec3_norm(vec3 r, const vec3 v);
void vec4_norm(vec4 r, const vec4 v);

void tri_norm(vec3 n, const vec3 p0, const vec3 p1, const vec3 p2);

float vec2_dist(const vec2 p0, const vec2 p1);
float vec3_dist(const vec3 p0, const vec3 p1);

/* r, a and b must never point to the same vector */
void vec3_cross(vec3 r, const vec3 a, const vec3 b);
void vec4_cross(vec4 r, const vec4 a, const vec4 b);

void vec3_reflect(vec3 r, const vec3 v, const vec3 n);
void vec4_reflect(vec4 r, const vec4 v, const vec4 n);

/* matrix */
void mat4_identity(mat4 m);
void mat4_dup(mat4 m, const mat4 n);
void mat4_row(vec4 r, const mat4 m, int i);
void mat4_col(vec4 r, const mat4 m, int i);

void mat4_transpose(mat4 m, const mat4 n);

void mat4_add(mat4 m, const mat4 a, const mat4 b);
void mat4_sub(mat4 m, const mat4 a, const mat4 b);
void mat4_scale(mat4 m, const mat4 a, float x, float y, float z);
void mat4_mul(mat4 m, const mat4 a, const mat4 b);

void mat4_translate(mat4 m, float x, float y, float z);
void mat4_translate_in_place(mat4 m, float x, float y, float z);

void mat4_rotate_x(mat4 q, const mat4 m, float angle);
void mat4_rotate_y(mat4 q, const mat4 m, float angle);
void mat4_rotate_z(mat4 q, const mat4 m, float angle);

void mat4_invert(mat4 t, const mat4 m);
void mat4_orthonormalize(mat4 r, const mat4 m);
void mat4_frustum(mat4 m, float l, float r, float b, float t, float n, float f);

void mat4_ortho(mat4 m, float l, float r, float b, float t, float n, float f);
void mat4_perspective(mat4 m, float y_fov, float aspect, float n, float f);
void mat4_look_at(mat4 m, const vec3 eye, const vec3 center, const vec3 up);

void mat4_mul_vec3(vec3 r, const mat4 m, const vec3 v);
void mat4_mul_vec4(vec4 r, const mat4 m, const vec4 v);

void mat4_print(const mat4 m);
void mat4_translation(const mat4 m);

#endif

