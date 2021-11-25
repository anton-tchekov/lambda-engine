#ifndef __SCENE_H__
#define __SCENE_H__

#include "mesh.h"
#include "lighting.h"

#define SCENE_FLAG_INVISIBLE 0x01
#define SCENE_FLAG_DISABLE   0x02
#define SCENE_FLAG_DIRTY     0x04
#define SCENE_FLAG_SHADOW    0x08
#define SCENE_FLAG_NO_SEL    0x10

typedef struct DYNAMIC_NODE
{
	mat4 ModelMatrix, PositionMatrix;
	struct DYNAMIC_NODE *Prev, *Next, *Children, *Parent;
	u32 SelectionID, MeshID, ShadowID, RenderCond, Flags;
} DynamicNode;

typedef struct SCENE
{
	Lighting *Lighting;
	DynamicNode Root;
} Scene;

void scene_init(Scene *scene, Lighting *lighting);
void scene_prepare(void);
void scene_render_lighting(void);
void scene_render_shadow(mat4 projection);
void scene_render_shadow_map(mat4 projection);
void scene_render_selection(mat4 projection);
void scene_render_selection_position(mat4 projection);
void scene_render_shadow_bw(mat4 projection);

void node_init(DynamicNode *node, u32 mesh_id, u32 shadow_id, u32 selection_id, u32 condition_id, u32 flags);
void node_add(DynamicNode *node, DynamicNode *child);
void node_invalidate(DynamicNode *node);

/* RESET */
void node_matrix(DynamicNode *node);
void node_reset(DynamicNode *node, float x, float y, float z);
void node_reset_v3(DynamicNode *node, vec3 p);

/* TRANSLATE */
void node_translate_x(DynamicNode *node, float x);
void node_translate_y(DynamicNode *node, float y);
void node_translate_z(DynamicNode *node, float z);
void node_translate_xy(DynamicNode *node, float x, float y);
void node_translate_xz(DynamicNode *node, float x, float z);
void node_translate_yz(DynamicNode *node, float y, float z);
void node_translate(DynamicNode *node, float x, float y, float z);
void node_translate_v3(DynamicNode *node, vec3 p);

/* ROTATE */
void node_rotate_x(DynamicNode *node, float r);
void node_rotate_y(DynamicNode *node, float r);
void node_rotate_z(DynamicNode *node, float r);
void node_rotate_1(DynamicNode *node, int a, float ra);
void node_rotate_2(DynamicNode *node, int a, float ra, int b, float rb);
void node_rotate_3(DynamicNode *node, int a, float ra, int b, float rb, int c, float rc);

/* SCALE */
void node_scale_x(DynamicNode *node, float x);
void node_scale_y(DynamicNode *node, float y);
void node_scale_z(DynamicNode *node, float z);
void node_scale_xz(DynamicNode *node, float x, float z);
void node_scale_xy(DynamicNode *node, float x, float y);
void node_scale_yz(DynamicNode *node, float y, float z);
void node_scale(DynamicNode *node, float x, float y, float z);
void node_scale_v3(DynamicNode *node, vec3 p);
void node_scale_uniform(DynamicNode *node, float x);

/* MUL */
void node_mul(DynamicNode *node, mat4 m);

/* CENTER */
void node_center(MeshCollection *mc, DynamicNode *node);

#endif

