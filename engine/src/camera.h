#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vector.h"

typedef struct CAMERA
{
	float NearPlane, FarPlane;
	mat4 Perspective, PerspectiveSkybox, PerspectiveEye[2];
	vec3 Position, Rotation;
	vec2 Front, Right, FOV;
} Camera;

void camera_init(Camera *camera, float fov, float near, float far);
void camera_update(Camera *camera);
void camera_on_mouse(Camera *camera, double offset_x, double offset_y);
void camera_perspective(Camera *camera, float scale);
void camera_fov_x(Camera *camera, float fov_x, float scale);
void camera_fov_y(Camera *camera, float fov_y, float scale);

#endif

