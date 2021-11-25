#include "lighting.h"
#include <assert.h>

void lighting_init(Lighting *lighting)
{
	int i;
	for(i = 0; i < NUM_POINT_LIGHTS; ++i)
	{
		lighting->PointLights.Position[i][X] = 0.0f;
		lighting->PointLights.Position[i][Y] = 0.0f;
		lighting->PointLights.Position[i][Z] = 0.0f;

		lighting->PointLights.Color[i][0] = 0.0f;
		lighting->PointLights.Color[i][1] = 0.0f;
		lighting->PointLights.Color[i][2] = 0.0f;

		lighting->PointLights.Constant[i] = 1.0f;
		lighting->PointLights.Linear[i] = 0.0f;
		lighting->PointLights.Quadratic[i] = 0.0f;
	}
}

void light_dir_position(Lighting *lighting, float x, float y, float z)
{
	lighting->DirLight.Position[X] = x;
	lighting->DirLight.Position[Y] = y;
	lighting->DirLight.Position[Z] = z;
}

void light_dir_position_v3(Lighting *lighting, const vec3 position)
{
	vec3_dup(lighting->DirLight.Position, position);
}

void light_dir_color(Lighting *lighting, float r, float g, float b)
{
	lighting->DirLight.Color[0] = r;
	lighting->DirLight.Color[1] = g;
	lighting->DirLight.Color[2] = b;
}

void light_dir_color_v3(Lighting *lighting, const vec3 color)
{
	vec3_dup(lighting->DirLight.Color, color);
}

void light_position(Lighting *lighting, int id, float x, float y, float z)
{
	assert(id >= 0 && id < NUM_POINT_LIGHTS);
	lighting->PointLights.Position[id][X] = x;
	lighting->PointLights.Position[id][Y] = y;
	lighting->PointLights.Position[id][Z] = z;
}

void light_position_v3(Lighting *lighting, int id, const vec3 position)
{
	assert(id >= 0 && id < NUM_POINT_LIGHTS);
	vec3_dup(lighting->PointLights.Position[id], position);
}

void light_color(Lighting *lighting, int id, float r, float g, float b)
{
	assert(id >= 0 && id < NUM_POINT_LIGHTS);
	lighting->PointLights.Color[id][0] = r;
	lighting->PointLights.Color[id][1] = g;
	lighting->PointLights.Color[id][2] = b;
}

void light_color_v3(Lighting *lighting, int id, const vec3 color)
{
	assert(id >= 0 && id < NUM_POINT_LIGHTS);
	vec3_dup(lighting->PointLights.Color[id], color);
}

void light_spread(Lighting *lighting, int id, float c, float l, float q)
{
	assert(id >= 0 && id < NUM_POINT_LIGHTS);
	lighting->PointLights.Constant[id] = c;
	lighting->PointLights.Linear[id] = l;
	lighting->PointLights.Quadratic[id] = q;
}
