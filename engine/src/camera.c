#include "camera.h"

void camera_init(Camera *camera, float fov, float near, float far)
{
	camera->FOV[X] = fov;
	camera->NearPlane = near;
	camera->FarPlane = far;

	camera->Position[X] = 0.0;
	camera->Position[Y] = 0.0;
	camera->Position[Z] = 0.0;
	camera_update(camera);
}

void camera_update(Camera *camera)
{
	float angle;
	camera->Right[X] = (float)cos(camera->Rotation[Y]);
	camera->Right[Y] = (float)sin(camera->Rotation[Y]);
	angle = camera->Rotation[Y] - (float)RAD(90.0f);
	camera->Front[X] = (float)cos(angle);
	camera->Front[Y] = (float)sin(angle);
}

void camera_on_mouse(Camera *camera, double offset_x, double offset_y)
{
	camera->Rotation[X] += (float)RAD(offset_y);
	camera->Rotation[X] = fminf(camera->Rotation[X], (float)RAD(89.0f));
	camera->Rotation[X] = fmaxf(camera->Rotation[X], (float)RAD(-89.0f));

	camera->Rotation[Y] += (float)RAD(offset_x);
	camera->Rotation[Y] = camera->Rotation[Y] - 2.0f * (float)PI * (float)floor(camera->Rotation[Y] / (2.0f * PI));
	camera_update(camera);
}

void camera_perspective(Camera *camera, float scale)
{
	mat4 projection, view, viewl, viewr, translate, tr, tl, rotate, rotx, roty, rotz;
	camera->FOV[Y] = camera->FOV[X] / scale;

	mat4_perspective(projection, camera->FOV[Y], scale, camera->NearPlane, camera->FarPlane);
	mat4_identity(rotx);
	mat4_rotate_x(rotx, rotx, -camera->Rotation[X]);

	mat4_identity(roty);
	mat4_rotate_y(roty, roty, -camera->Rotation[Y]);

	mat4_identity(rotz);
	mat4_rotate_z(rotz, rotz, -camera->Rotation[Z]);

	mat4_identity(rotate);
	mat4_mul(rotate, rotate, rotz);
	mat4_mul(rotate, rotate, rotx);
	mat4_mul(rotate, rotate, roty);

	mat4_translate(translate, -camera->Position[X], -camera->Position[Y], -camera->Position[Z]);

	mat4_translate(tl, -camera->Position[X] - camera->Right[X] * 0.038f, -camera->Position[Y], -camera->Position[Z] - camera->Right[Y] * 0.038f);
	mat4_translate(tr, -camera->Position[X] + camera->Right[X] * 0.038f, -camera->Position[Y], -camera->Position[Z] + camera->Right[Y] * 0.038f);

	mat4_mul(view, rotate, translate);
	mat4_mul(camera->Perspective, projection, view);

	mat4_mul(viewl, rotate, tl);
	mat4_mul(camera->PerspectiveEye[0], projection, viewl);

	mat4_mul(viewr, rotate, tr);
	mat4_mul(camera->PerspectiveEye[1], projection, viewr);

	mat4_mul(camera->PerspectiveSkybox, projection, rotate);
}

