#ifndef __RENDERER3D_H__
#define __RENDERER3D_H__

#include "internal.h"
#include "mesh.h"
#include "shadow.h"
#include "camera.h"
#include "scene.h"
#include "physics.h"

int renderer3d_init(void);
void renderer3d_dsl(float dark, float shadow, float light);
void renderer3d_bind_mesh_collection(MeshCollection *mc);
void renderer3d_bind_shadow_collection(ShadowCollection *sc);
void renderer3d_destroy(void);

void renderer3d_bind_scene(Scene *scene);
void renderer3d_bind_camera(Camera *camera);
void renderer3d_bind_world(World *world);

int renderer3d_render(void);

void renderer3d_fbo_destroy(void);
void renderer3d_dimensions(int w, int h);

#endif

