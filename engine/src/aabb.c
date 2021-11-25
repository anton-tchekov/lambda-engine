#include "aabb.h"
#include <stdio.h>
#include <float.h>

float aabb_size_x(AABB *a)
{
	return a->Max[X] - a->Min[X];
}

float aabb_size_y(AABB *a)
{
	return a->Max[Y] - a->Min[Y];
}

float aabb_size_z(AABB *a)
{
	return a->Max[Z] - a->Min[Z];
}

void aabb_normalize(AABB *a)
{
	if(a->Min[X] > a->Max[X])
	{
		float temp = a->Max[X];
		a->Max[X] = a->Min[X];
		a->Min[X] = temp;
	}

	if(a->Min[Y] > a->Max[Y])
	{
		float temp = a->Max[Y];
		a->Max[Y] = a->Min[Y];
		a->Min[Y] = temp;
	}

	if(a->Min[Z] > a->Max[Z])
	{
		float temp = a->Max[Z];
		a->Max[Z] = a->Min[Z];
		a->Min[Z] = temp;
	}
}

void aabb_print(AABB *a)
{
	printf(
		"Printing AABB:\n"
		"a->Min[X] = %f\n"
		"a->Min[Y] = %f\n"
		"a->Min[Z] = %f\n\n"
		"a->Max[X] = %f\n"
		"a->Max[Y] = %f\n"
		"a->Max[Z] = %f\n"
		"a->VelocY = %f\n\n",
			a->Min[X], a->Min[Y], a->Min[Z],
			a->Max[X], a->Max[Y], a->Max[Z],
			a->VelocityY);
}

void aabb_enclose(AABB *out, AABB *in, int count)
{
	int i;
	out->Min[X] = FLT_MAX;
	out->Min[Y] = FLT_MAX;
	out->Min[Z] = FLT_MAX;

	out->Max[X] = -FLT_MAX;
	out->Max[Y] = -FLT_MAX;
	out->Max[Z] = -FLT_MAX;

	for(i = 0; i < count; ++i)
	{
		/* Min */
		if(in[i].Min[X] < out->Min[X])
		{
			out->Min[X] = in[i].Min[X];
		}

		if(in[i].Min[Y] < out->Min[Y])
		{
			out->Min[Y] = in[i].Min[Y];
		}

		if(in[i].Min[Z] < out->Min[Z])
		{
			out->Min[Z] = in[i].Min[Z];
		}

		/* Max */
		if(in[i].Max[X] > out->Max[X])
		{
			out->Max[X] = in[i].Max[X];
		}

		if(in[i].Max[Y] > out->Max[Y])
		{
			out->Max[Y] = in[i].Max[Y];
		}

		if(in[i].Max[Z] > out->Max[Z])
		{
			out->Max[Z] = in[i].Max[Z];
		}
	}
}

void aabb_transform(AABB *out, AABB *in, mat4 model)
{
	out->Min[X] = model[0][0] * in->Min[0] + model[1][0] * in->Min[1] + model[2][0] * in->Min[2] + model[3][0];
	out->Min[Y] = model[0][1] * in->Min[0] + model[1][1] * in->Min[1] + model[2][1] * in->Min[2] + model[3][1];
	out->Min[Z] = model[0][2] * in->Min[0] + model[1][2] * in->Min[1] + model[2][2] * in->Min[2] + model[3][2];

	out->Max[X] = model[0][0] * in->Max[0] + model[1][0] * in->Max[1] + model[2][0] * in->Max[2] + model[3][0];
	out->Max[Y] = model[0][1] * in->Max[0] + model[1][1] * in->Max[1] + model[2][1] * in->Max[2] + model[3][1];
	out->Max[Z] = model[0][2] * in->Max[0] + model[1][2] * in->Max[1] + model[2][2] * in->Max[2] + model[3][2];
}

int aabb_intersect(AABB *a, AABB *b)
{
	return a->Min[X] <= b->Max[X] && a->Max[X] >= b->Min[X] &&
			a->Min[Y] <= b->Max[Y] && a->Max[Y] >= b->Min[Y] &&
			a->Min[Z] <= b->Max[Z] && a->Max[Z] >= b->Min[Z];
}

void aabb_instance(AABB *out, AABB *in, mat4 model)
{
	u32 i;
	vec3 corners[8] =
	{
		{ in->Min[X], in->Min[Y], in->Min[Z] },
		{ in->Max[X], in->Min[Y], in->Min[Z] },
		{ in->Min[X], in->Max[Y], in->Min[Z] },
		{ in->Min[X], in->Min[Y], in->Max[Z] },
		{ in->Max[X], in->Max[Y], in->Min[Z] },
		{ in->Min[X], in->Max[Y], in->Max[Z] },
		{ in->Max[X], in->Min[Y], in->Max[Z] },
		{ in->Max[X], in->Max[Y], in->Max[Z] }
	};

	out->Min[X] = FLT_MAX;
	out->Min[Y] = FLT_MAX;
	out->Min[Z] = FLT_MAX;

	out->Max[X] = -FLT_MAX;
	out->Max[Y] = -FLT_MAX;
	out->Max[Z] = -FLT_MAX;
	for(i = 0; i < 8; ++i)
	{
		vec3 pos;
		pos[X] = model[0][0] * corners[i][X] + model[1][0] * corners[i][Y] + model[2][0] * corners[i][Z] + model[3][0];
		pos[Y] = model[0][1] * corners[i][X] + model[1][1] * corners[i][Y] + model[2][1] * corners[i][Z] + model[3][1];
		pos[Z] = model[0][2] * corners[i][X] + model[1][2] * corners[i][Y] + model[2][2] * corners[i][Z] + model[3][2];

		if(pos[X] < out->Min[X])
		{
			out->Min[X] = pos[X];
		}

		if(pos[X] > out->Max[X])
		{
			out->Max[X] = pos[X];
		}

		if(pos[Y] < out->Min[Y])
		{
			out->Min[Y] = pos[Y];
		}

		if(pos[Y] > out->Max[Y])
		{
			out->Max[Y] = pos[Y];
		}

		if(pos[Z] < out->Min[Z])
		{
			out->Min[Z] = pos[Z];
		}

		if(pos[Z] > out->Max[Z])
		{
			out->Max[Z] = pos[Z];
		}
	}
}

