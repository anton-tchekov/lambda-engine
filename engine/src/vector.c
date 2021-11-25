#include "vector.h"
#include <stdio.h>

i32 abs(i32 a)
{
	return a < 0 ? -a : a;
}

void vec2_add(vec2 r, const vec2 a, const vec2 b)
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
}

void vec3_add(vec3 r, const vec3 a, const vec3 b)
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
}

void vec4_add(vec4 r, const vec4 a, const vec4 b)
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
	r[3] = a[3] + b[3];
}

void vec2_sub(vec2 r, const vec2 a, const vec2 b)
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
}

void vec3_sub(vec3 r, const vec3 a, const vec3 b)
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
}

void vec4_sub(vec4 r, const vec4 a, const vec4 b)
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
	r[3] = a[3] - b[3];
}

void vec2_scale(vec2 r, const vec2 v, float s)
{
	r[0] = v[0] * s;
	r[1] = v[1] * s;
}

void vec3_scale(vec3 r, const vec3 v, float s)
{
	r[0] = v[0] * s;
	r[1] = v[1] * s;
	r[2] = v[2] * s;
}

void vec4_scale(vec4 r, const vec4 v, float s)
{
	r[0] = v[0] * s;
	r[1] = v[1] * s;
	r[2] = v[2] * s;
	r[3] = v[3] * s;
}

void vec2_dup(vec2 r, const vec2 v)
{
	r[0] = v[0];
	r[1] = v[1];
}

void vec3_dup(vec3 r, const vec3 v)
{
	r[0] = v[0];
	r[1] = v[1];
	r[2] = v[2];
}

void vec4_dup(vec4 r, const vec4 v)
{
	r[0] = v[0];
	r[1] = v[1];
	r[2] = v[2];
	r[3] = v[3];
}

float vec2_dot(const vec2 a, const vec2 b)
{
	return b[0] * a[0] + b[1] * a[1];
}

float vec3_dot(const vec3 a, const vec3 b)
{
	return b[0] * a[0] + b[1] * a[1] + b[2] * a[2];
}

float vec4_dot(const vec4 a, const vec4 b)
{
	return b[0] * a[0] + b[1] * a[1] + b[2] * a[2] + b[3] * a[3];
}

float vec2_len(const vec2 v)
{
	return (float)sqrt(vec2_dot(v, v));
}

float vec3_len(const vec3 v)
{
	return (float)sqrt(vec3_dot(v, v));
}

float vec4_len(const vec4 v)
{
	return (float)sqrt(vec4_dot(v, v));
}

void vec2_norm(vec2 r, const vec2 v)
{
	float k = 1.0f / vec2_len(v);
	vec2_scale(r, v, k);
}

void vec3_norm(vec3 r, const vec3 v)
{
	float k = 1.0f / vec3_len(v);
	vec3_scale(r, v, k);
}

void vec4_norm(vec4 r, const vec4 v)
{
	float k = 1.0f / vec4_len(v);
	vec4_scale(r, v, k);
}

void tri_norm(vec3 n, const vec3 p0, const vec3 p1, const vec3 p2)
{
	vec3 u, v;
	vec3_sub(u, p1, p0);
	vec3_sub(v, p2, p0);
	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
}

float vec2_dist(const vec2 p0, const vec2 p1)
{
	vec2 d;
	d[X] = p1[X] - p0[X];
	d[Y] = p1[Y] - p0[Y];
	return (float)sqrt(d[X] * d[X] + d[Y] * d[Y]);
}

float vec3_dist(const vec3 p0, const vec3 p1)
{
	vec3 d;
	d[X] = p1[X] - p0[X];
	d[Y] = p1[Y] - p0[Y];
	d[Z] = p1[Z] - p0[Z];
	return (float)sqrt(d[X] * d[X] + d[Y] * d[Y] + d[Z] * d[Z]);
}

void vec3_cross(vec3 r, const vec3 a, const vec3 b)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

void vec4_cross(vec4 r, const vec4 a, const vec4 b)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
	r[3] = 1.0f;
}

void vec3_reflect(vec3 r, const vec3 v, const vec3 n)
{
	float p;
	p = 2.0f * vec3_dot(v, n);
	r[0] = v[0] - p * n[0];
	r[1] = v[1] - p * n[1];
	r[2] = v[2] - p * n[2];
}

void vec4_reflect(vec4 r, const vec4 v, const vec4 n)
{
	float p;
	p = 2.0f * vec4_dot(v, n);
	r[0] = v[0] - p * n[0];
	r[1] = v[1] - p * n[1];
	r[2] = v[2] - p * n[2];
	r[3] = v[3] - p * n[3];
}

/* matrix */
void mat4_identity(mat4 m)
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void mat4_dup(mat4 m, const mat4 n)
{
	vec4_dup(m[0], n[0]);
	vec4_dup(m[1], n[1]);
	vec4_dup(m[2], n[2]);
	vec4_dup(m[3], n[3]);
}

void mat4_row(vec4 r, const mat4 m, int i)
{
	r[0] = m[0][i];
	r[1] = m[1][i];
	r[2] = m[2][i];
	r[3] = m[3][i];
}

void mat4_col(vec4 r, const mat4 m, int i)
{
	r[0] = m[i][0];
	r[1] = m[i][1];
	r[2] = m[i][2];
	r[3] = m[i][3];
}

void mat4_transpose(mat4 m, const mat4 n)
{
	m[0][0] = n[0][0];
	m[1][0] = n[0][1];
	m[2][0] = n[0][2];
	m[3][0] = n[0][3];

	m[0][1] = n[1][0];
	m[1][1] = n[1][1];
	m[2][1] = n[1][2];
	m[3][1] = n[1][3];

	m[0][2] = n[2][0];
	m[1][2] = n[2][1];
	m[2][2] = n[2][2];
	m[3][2] = n[2][3];

	m[0][3] = n[3][0];
	m[1][3] = n[3][1];
	m[2][3] = n[3][2];
	m[3][3] = n[3][3];
}

void mat4_add(mat4 m, const mat4 a, const mat4 b)
{
	vec4_add(m[0], a[0], b[0]);
	vec4_add(m[1], a[1], b[1]);
	vec4_add(m[2], a[2], b[2]);
	vec4_add(m[3], a[3], b[3]);
}

void mat4_sub(mat4 m, const mat4 a, const mat4 b)
{
	vec4_sub(m[0], a[0], b[0]);
	vec4_sub(m[1], a[1], b[1]);
	vec4_sub(m[2], a[2], b[2]);
	vec4_sub(m[3], a[3], b[3]);
}

void mat4_scale(mat4 m, const mat4 a, float x, float y, float z)
{
	vec4_scale(m[0], a[0], x);
	vec4_scale(m[1], a[1], y);
	vec4_scale(m[2], a[2], z);
	m[3][0] = a[3][0];
	m[3][1] = a[3][1];
	m[3][2] = a[3][2];
	m[3][3] = a[3][3];
}

void mat4_mul(mat4 m, const mat4 a, const mat4 b)
{
	mat4 temp;
	int r, c;
	for(c = 0; c < 4; ++c)
	{
		for(r = 0; r < 4; ++r)
		{
			temp[c][r] =
					a[0][r] * b[c][0] +
					a[1][r] * b[c][1] +
					a[2][r] * b[c][2] +
					a[3][r] * b[c][3];
		}
	}

	mat4_dup(m, temp);
}

void mat4_translate(mat4 m, float x, float y, float z)
{
	mat4_identity(m);
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

void mat4_translate_in_place(mat4 m, float x, float y, float z)
{
	vec4 r, t = { x, y, z, 0 };

	mat4_row(r, m, 0);
	m[3][0] += vec4_dot(r, t);

	mat4_row(r, m, 1);
	m[3][1] += vec4_dot(r, t);

	mat4_row(r, m, 2);
	m[3][2] += vec4_dot(r, t);

	mat4_row(r, m, 3);
	m[3][3] += vec4_dot(r, t);
}

/* Rotate */
void mat4_rotate_x(mat4 q, const mat4 m, float angle)
{
	float s = sinf(angle), c = cosf(angle);
	mat4 r =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f,   c,    s,  0.0f },
		{ 0.0f,  -s,    c,  0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	mat4_mul(q, m, r);
}

void mat4_rotate_y(mat4 q, const mat4 m, float angle)
{
	float s = sinf(angle), c = cosf(angle);
	mat4 r =
	{
		{   c,  0.0f,   s,  0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{  -s,  0.0f,   c,  0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	mat4_mul(q, m, r);
}

void mat4_rotate_z(mat4 q, const mat4 m, float angle)
{
	float s = sinf(angle), c = cosf(angle);
	mat4 r =
	{
		{   c,    s,  0.0f, 0.0f },
		{  -s,    c,  0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	mat4_mul(q, m, r);
}

void mat4_invert(mat4 t, const mat4 m)
{
	float idet, s[6], c[6];
	s[0] = m[0][0] * m[1][1] - m[1][0] * m[0][1];
	s[1] = m[0][0] * m[1][2] - m[1][0] * m[0][2];
	s[2] = m[0][0] * m[1][3] - m[1][0] * m[0][3];
	s[3] = m[0][1] * m[1][2] - m[1][1] * m[0][2];
	s[4] = m[0][1] * m[1][3] - m[1][1] * m[0][3];
	s[5] = m[0][2] * m[1][3] - m[1][2] * m[0][3];

	c[0] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	c[1] = m[2][0] * m[3][2] - m[3][0] * m[2][2];
	c[2] = m[2][0] * m[3][3] - m[3][0] * m[2][3];
	c[3] = m[2][1] * m[3][2] - m[3][1] * m[2][2];
	c[4] = m[2][1] * m[3][3] - m[3][1] * m[2][3];
	c[5] = m[2][2] * m[3][3] - m[3][2] * m[2][3];

	idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

	t[0][0] = ( m[1][1] * c[5] - m[1][2] * c[4] + m[1][3] * c[3]) * idet;
	t[0][1] = (-m[0][1] * c[5] + m[0][2] * c[4] - m[0][3] * c[3]) * idet;
	t[0][2] = ( m[3][1] * s[5] - m[3][2] * s[4] + m[3][3] * s[3]) * idet;
	t[0][3] = (-m[2][1] * s[5] + m[2][2] * s[4] - m[2][3] * s[3]) * idet;

	t[1][0] = (-m[1][0] * c[5] + m[1][2] * c[2] - m[1][3] * c[1]) * idet;
	t[1][1] = ( m[0][0] * c[5] - m[0][2] * c[2] + m[0][3] * c[1]) * idet;
	t[1][2] = (-m[3][0] * s[5] + m[3][2] * s[2] - m[3][3] * s[1]) * idet;
	t[1][3] = ( m[2][0] * s[5] - m[2][2] * s[2] + m[2][3] * s[1]) * idet;

	t[2][0] = ( m[1][0] * c[4] - m[1][1] * c[2] + m[1][3] * c[0]) * idet;
	t[2][1] = (-m[0][0] * c[4] + m[0][1] * c[2] - m[0][3] * c[0]) * idet;
	t[2][2] = ( m[3][0] * s[4] - m[3][1] * s[2] + m[3][3] * s[0]) * idet;
	t[2][3] = (-m[2][0] * s[4] + m[2][1] * s[2] - m[2][3] * s[0]) * idet;

	t[3][0] = (-m[1][0] * c[3] + m[1][1] * c[1] - m[1][2] * c[0]) * idet;
	t[3][1] = ( m[0][0] * c[3] - m[0][1] * c[1] + m[0][2] * c[0]) * idet;
	t[3][2] = (-m[3][0] * s[3] + m[3][1] * s[1] - m[3][2] * s[0]) * idet;
	t[3][3] = ( m[2][0] * s[3] - m[2][1] * s[1] + m[2][2] * s[0]) * idet;
}

void mat4_orthonormalize(mat4 r, const mat4 m)
{
	float s = 1.0f;
	vec3 h;

	mat4_dup(r, m);
	vec3_norm(r[2], r[2]);

	s = vec3_dot(r[1], r[2]);
	vec3_scale(h, r[2], s);
	vec3_sub(r[1], r[1], h);
	vec3_norm(r[2], r[2]);

	s = vec3_dot(r[1], r[2]);
	vec3_scale(h, r[2], s);
	vec3_sub(r[1], r[1], h);
	vec3_norm(r[1], r[1]);

	s = vec3_dot(r[0], r[1]);
	vec3_scale(h, r[1], s);
	vec3_sub(r[0], r[0], h);
	vec3_norm(r[0], r[0]);
}

void mat4_frustum(mat4 m, float l, float r, float b, float t, float n, float f)
{
	m[0][0] = 2.0f * n / (r - l);
	m[0][1] = m[0][2] = m[0][3] = 0.0f;

	m[1][1] = 2.0f * n / (t - b);
	m[1][0] = m[1][2] = m[1][3] = 0.0f;

	m[2][0] = (r + l) / (r - l);
	m[2][1] = (t + b) / (t - b);
	m[2][2] = -(f + n) / (f- n);
	m[2][3] = -1.0f;

	m[3][2] = -2.0f * (f * n) / (f - n);
	m[3][0] = m[3][1] = m[3][3] = 0.0f;
}

void mat4_ortho(mat4 m, float l, float r, float b, float t, float n, float f)
{
	m[0][0] = 2.0f / (r - l);
	m[0][1] = m[0][2] = m[0][3] = 0.0f;

	m[1][1] = 2.0f / (t - b);
	m[1][0] = m[1][2] = m[1][3] = 0.0f;

	m[2][2] = -2.0f / (f - n);
	m[2][0] = m[2][1] = m[2][3] = 0.0f;

	m[3][0] = -(r + l) / (r - l);
	m[3][1] = -(t + b) / (t - b);
	m[3][2] = -(f + n) / (f - n);
	m[3][3] = 1.0f;
}

void mat4_perspective(mat4 m, float y_fov, float aspect, float n, float f)
{
	float a = 1.0f / (float)tan(y_fov / 2.0f);

	m[0][0] = a / aspect;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = a;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = -((f + n) / (f - n));
	m[2][3] = -1.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = -((2.0f * f * n) / (f - n));
	m[3][3] = 0.0f;
}


void mat4_look_at(mat4 m, const vec3 eye, const vec3 center, const vec3 up)
{
	vec3 f, s, t;

	vec3_sub(f, center, eye);
	vec3_norm(f, f);

	vec3_cross(s, f, up);
	vec3_norm(s, s);

	vec3_cross(t, s, f);

	m[0][0] = s[0];
	m[0][1] = t[0];
	m[0][2] = -f[0];
	m[0][3] = 0.0f;

	m[1][0] = s[1];
	m[1][1] = t[1];
	m[1][2] = -f[1];
	m[1][3] = 0.0f;

	m[2][0] = s[2];
	m[2][1] = t[2];
	m[2][2] = -f[2];
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	mat4_translate_in_place(m, -eye[0], -eye[1], -eye[2]);
}



/*void mat4_look_at(mat4 m, const vec3 eye, const vec3 center, const vec3 up)
{
	vec3 f, u, s;

	vec3_sub(f, center, eye);
	vec3_norm(f, f);

	vec3_norm(u, up);

	vec3_cross(s, f, u);
	vec3_norm(s, s);

	vec3_cross(u, s, f);

	mat4_identity(m);

	m[0][0] = s[X];
	m[1][0] = s[Y];
	m[2][0] = s[Z];

	m[0][1] = u[X];
	m[1][1] = u[Y];
	m[2][1] = u[Z];

	m[0][2] = -f[X];
	m[1][2] = -f[Y];
	m[2][2] = -f[Z];

	m[3][0] = -vec3_dot(s, eye);
	m[3][1] = -vec3_dot(u, eye);
	m[3][2] =  vec3_dot(f, eye);
}*/






void mat4_mul_vec3(vec3 r, const mat4 m, const vec3 v)
{
	r[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0];
	r[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1];
	r[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2];
}

void mat4_mul_vec4(vec4 r, const mat4 m, const vec4 v)
{
	r[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	r[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	r[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	r[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
}

void mat4_print(const mat4 m)
{
	int i;
	for(i = 0; i < 4; ++i)
	{
		printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
	}
}

void mat4_translation(const mat4 m)
{
	printf("Translation: %f %f %f\n", m[3][0], m[3][1], m[3][2]);
}

