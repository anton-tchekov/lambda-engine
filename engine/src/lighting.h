#ifndef __LIGHTING_H__
#define __LIGHTING_H__

#include "vector.h"

#define NUM_POINT_LIGHTS    4

typedef struct DIR_LIGHT
{
	vec3 Position, Color;
} DirLight;

typedef struct POINT_LIGHTS
{
	vec3 Position[NUM_POINT_LIGHTS],
		Color[NUM_POINT_LIGHTS];

	float Constant[NUM_POINT_LIGHTS],
		Linear[NUM_POINT_LIGHTS],
		Quadratic[NUM_POINT_LIGHTS];
} PointLights;

typedef struct LIGHTING
{
	DirLight DirLight;
	PointLights PointLights;
} Lighting;

void lighting_init(Lighting *lighting);

void light_dir_position(Lighting *lighting, float x, float y, float z);
void light_dir_position_v3(Lighting *lighting, const vec3 position);
void light_dir_color(Lighting *lighting, float r, float g, float b);
void light_dir_color_v3(Lighting *lighting, const vec3 color);

void light_position_v3(Lighting *lighting, int id, const vec3 position);
void light_color_v3(Lighting *lighting, int id, const vec3 color);
void light_position(Lighting *lighting, int id, float x, float y, float z);
void light_color(Lighting *lighting, int id, float r, float g, float b);
void light_spread(Lighting *lighting, int id, float c, float l, float q);

#endif

