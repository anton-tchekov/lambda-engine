#include "scene.h"
#include "opengl.h"
#include "internal.h"
#include "debug.h"
#include "log.h"
#include "condition.h"
#include <stdlib.h>
#include <assert.h>

#define SCENE_FLAG_INIT (1 << 16)
#define SCENE_FLAG_ADD  (1 << 17)

#define MEM 4096

static void _render_node_shadow(DynamicNode *node);
static void _prepare_node(mat4 model, DynamicNode *node, int dirty);

/* Selection */
static float _sid[MEM];
static u32 _ss[MEM], _smo[MEM], _smc[MEM], _smt[MEM], _si;
static DynamicNode *_snode[MEM];
static mat4 _spm[MEM];

/* Lighting */
static u32 _ms[MEM], _mt[MEM], _mi;
static DynamicNode *_node[MEM];
static mat4 _mpm[MEM];

/* Save stack in recursion */
static GLint _uniform_position;

/* SCENE GRAPH */
void scene_init(Scene *scene, Lighting *lighting)
{
	scene->Lighting = lighting;
	node_init(&scene->Root, 0, 0, 0, 0, SCENE_FLAG_INVISIBLE);
	scene->Root.Prev = NULL;
	scene->Root.Next = NULL;
}

static void _scene_update_node(DynamicNode *node, mat4 model)
{
	node->Flags &= (u8)~SCENE_FLAG_DIRTY;
	mat4_mul(node->PositionMatrix, model, node->ModelMatrix);
}

void node_init(DynamicNode *node, u32 mesh_id, u32 shadow_id, u32 selection_id, u32 condition_id, u32 flags)
{
	node->ShadowID = shadow_id;
	node->MeshID = mesh_id;
	node->SelectionID = selection_id;
	node->Flags = flags | SCENE_FLAG_INIT;
	node->Children = NULL;
	node->RenderCond = condition_id;
	node_invalidate(node);
}

void node_add(DynamicNode *node, DynamicNode *child)
{
	DynamicNode *old;

	assert(node);
	assert(child);

	child->Flags |= SCENE_FLAG_ADD;

	if(node->Children == NULL)
	{
		node->Children = child;
		child->Next = NULL;
		child->Prev = NULL;
		return;
	}

	old = node->Children;
	node->Children = child;
	child->Next = old;
	old->Prev = node;
}

void node_invalidate(DynamicNode *node)
{
	node->Flags |= SCENE_FLAG_DIRTY;
}

/* SCENE PREPARE */
static int _mesh_compare(const void *p0, const void *p1)
{
	u32 a, b;
	a = _smt[*(const int *)p0];
	b = _smt[*(const int *)p1];

	if(a < b)
	{
		return -1;
	}

	if(a == b)
	{
		return 0;
	}

	return 1;
}

static int _material_compare(const void *p0, const void *p1)
{
	u32 a, b;
	a = _mt[*(const int *)p0];
	b = _mt[*(const int *)p1];

	if(a < b)
	{
		return -1;
	}

	if(a == b)
	{
		return 0;
	}

	return 1;
}

void scene_prepare(void)
{
	u32 i;
	mat4 model;
	DynamicNode *node;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	Scene *scene = renderer->Scene;

	_si = 0;
	_mi = 0;

	mat4_identity(model);
	for(node = scene->Root.Children; node; node = node->Next)
	{
		_prepare_node(model, node, 0);
	}

	/* Selection */
	for(i = 0; i < _si; ++i)
	{
		_ss[i] = i;
	}

	qsort(&_ss, _si, sizeof(u32), &_mesh_compare);

	for(i = 0; i < _si; ++i)
	{
		mat4_dup(_spm[i], _snode[_ss[i]]->PositionMatrix);
		_sid[i] = (float)_snode[_ss[i]]->SelectionID / (float)(0xFFFF - 1);
	}

	/* Lighting */
	for(i = 0; i < _mi; ++i)
	{
		_ms[i] = i;
	}

	qsort(&_ms, _mi, sizeof(u32), &_material_compare);

	for(i = 0; i < _mi; ++i)
	{
		mat4_dup(_mpm[i], _node[_ms[i]]->PositionMatrix);
	}
}

static void _prepare_node(mat4 model, DynamicNode *node, int dirty)
{
	DynamicNode *child;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	if(node->Flags & SCENE_FLAG_DISABLE)
	{
		return;
	}

	if(dirty || node->Flags & SCENE_FLAG_DIRTY)
	{
		dirty = 1;
		_scene_update_node(node, model);
	}

	if(node->RenderCond == 0 || condition_get(node->RenderCond))
	{
		if(!(node->Flags & SCENE_FLAG_INVISIBLE))
		{
			u32 i, mb, me, offset, count;
			mb = renderer->Meshes->MaterialOffsets[node->MeshID];
			me = mb + renderer->Meshes->MaterialCounts[node->MeshID] - 1;

			offset = renderer->Meshes->Materials[mb].VertexOffset;
			count = renderer->Meshes->Materials[me].VertexOffset - offset +
					renderer->Meshes->Materials[me].VertexCount;

			if(!(node->Flags & SCENE_FLAG_NO_SEL))
			{
				_smo[_si] = offset;
				_smc[_si] = count;
				_smt[_si] = mb;
				_snode[_si] = node;
				++_si;
			}

			me = mb + renderer->Meshes->MaterialCounts[node->MeshID];
			for(i = mb; i < me; ++i)
			{
				Material *m = &renderer->Meshes->Materials[i];
				if(m->VertexCount == 0)
				{
					continue;
				}

				_mt[_mi] = i;
				_node[_mi] = node;
				++_mi;
			}
		}

		for(child = node->Children; child; child = child->Next)
		{
			_prepare_node(node->PositionMatrix, child, dirty);
		}
	}
}

/* RENDER LIGHTING DYNAMIC */
static void _render_material(Material *m, u32 count, mat4 *pm)
{
	u32 i, rest;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	for(i = 0; i < count; i += 32)
	{
		rest = ((i + 32) < count) ? (32) : (count - i);

		if(m->LayerDiffuse == 0 && m->SizeDiffuse == 0)
		{
			GL_CHECK(glUniform3f(renderer->UniformLighting_ColorDiffuse, m->ColorDiffuse[0] / 255.0f, m->ColorDiffuse[1] / 255.0f, m->ColorDiffuse[2] / 255.0f));
		}
		else
		{
			GL_CHECK(glUniform3f(renderer->UniformLighting_ColorDiffuse, 1.0f, 1.0f, 1.0f));
		}

		if(m->LayerSpecular == 0 && m->SizeSpecular == 0)
		{
			GL_CHECK(glUniform3f(renderer->UniformLighting_ColorSpecular, m->ColorSpecular[0] / 255.0f, m->ColorSpecular[1] / 255.0f, m->ColorSpecular[2] / 255.0f));
		}
		else
		{
			GL_CHECK(glUniform3f(renderer->UniformLighting_ColorSpecular, 0.0f, 0.0f, 0.0f));
		}

		if(m->LayerEmissive == 0 && m->SizeEmissive == 0)
		{
			GL_CHECK(glUniform3f(renderer->UniformLighting_ColorEmissive, m->ColorEmissive[0] / 255.0f, m->ColorEmissive[1] / 255.0f, m->ColorEmissive[2] / 255.0f));
		}
		else
		{
			GL_CHECK(glUniform3f(renderer->UniformLighting_ColorEmissive, 1.0f, 1.0f, 1.0f));
		}

		GL_CHECK(glUniform1f(renderer->UniformLighting_Shininess, m->Shininess));

		GL_CHECK(glUniform1i(renderer->UniformLighting_SamplerDiffuse, (GLint)(TU_ARRAY + m->SizeDiffuse)));
		GL_CHECK(glUniform1i(renderer->UniformLighting_SamplerSpecular, (GLint)(TU_ARRAY + m->SizeSpecular)));
		GL_CHECK(glUniform1i(renderer->UniformLighting_SamplerEmissive, (GLint)(TU_ARRAY + m->SizeEmissive)));
		GL_CHECK(glUniform1i(renderer->UniformLighting_SamplerNormal, (GLint)(TU_ARRAY + m->SizeNormal)));

		GL_CHECK(glUniform1f(renderer->UniformLighting_LayerDiffuse, (float)m->LayerDiffuse));
		GL_CHECK(glUniform1f(renderer->UniformLighting_LayerSpecular, (float)m->LayerSpecular));
		GL_CHECK(glUniform1f(renderer->UniformLighting_LayerEmissive, (float)m->LayerEmissive));
		GL_CHECK(glUniform1f(renderer->UniformLighting_LayerNormal, (float)m->LayerNormal));

		GL_CHECK(glUniformMatrix4fv(_uniform_position, (GLsizei)rest, GL_FALSE, (const GLfloat *)&pm[i]));
		GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, (GLint)m->VertexOffset, (GLsizei)m->VertexCount, (GLsizei)rest));
	}
}

void scene_render_lighting(void)
{
	u32 i, start, count;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;

	_uniform_position = renderer->UniformLighting_PositionMatrix;

	u32 n = 0;
	start = 0;
	count = 1;
	for(i = 1; i < _mi; ++i)
	{
		if(_mt[_ms[i - 1]] != _mt[_ms[i]])
		{
			Material *m = &renderer->Meshes->Materials[_mt[_ms[start]]];
			_render_material(m, count, &_mpm[start]);
			start = i;
			count = 0;
			++n;
		}

		++count;
	}

	if(_mi)
	{
		Material *m = &renderer->Meshes->Materials[_mt[_ms[start]]];
		_render_material(m, count, &_mpm[start]);
	}
}

/* RENDER SHADOW BW */
static void _render_shadow_bw(u32 offset, u32 count, mat4 *m)
{
	GL_CHECK(glUniformMatrix4fv(_uniform_position, (GLsizei)count, GL_FALSE, (const GLfloat *)m));
	GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, (GLint)_smo[offset], (GLsizei)_smc[offset], (GLsizei)count));
}

void scene_render_shadow_bw(mat4 projection)
{
	u32 i, start, count;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glUniformMatrix4fv(renderer->UniformShadowBW_Perspective, 1, GL_FALSE, &projection[0][0]));
	_uniform_position = renderer->UniformShadowBW_PositionMatrix;

	start = 0;
	count = 1;
	for(i = 1; i < _si; ++i)
	{
		if(_smo[_ss[i - 1]] != _smo[_ss[i]])
		{
			_render_shadow_bw(_ss[start], count, &_spm[start]);
			start = i;
			count = 0;
		}

		++count;
	}

	if(_si)
	{
		_render_shadow_bw(_ss[start], count, &_spm[start]);
	}
}

/* RENDER SHADOW */
void scene_render_shadow(mat4 projection)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	DynamicNode *node;
	Scene *scene;
	scene = renderer->Scene;
	GL_CHECK(glUniformMatrix4fv(renderer->UniformShadowPerspective, 1, GL_FALSE, &projection[0][0]));

	renderer->ShadowNumVertices = 0;
	renderer->ShadowNumIndices = 0;

	for(node = scene->Root.Children; node; node = node->Next)
	{
		_render_node_shadow(node);
	}

	if(renderer->ShadowNumIndices)
	{
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, renderer->ShadowVBO[0]));
		GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(renderer->ShadowNumVertices * sizeof(vec3)), renderer->ShadowVertices));

		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ShadowVBO[1]));
		GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (GLsizeiptr)(renderer->ShadowNumIndices * sizeof(u32)), renderer->ShadowIndices));

		GL_CHECK(glBindVertexArray(renderer->ShadowVAO));
		GL_CHECK(glDrawElements(GL_TRIANGLES, (GLsizei)renderer->ShadowNumIndices, GL_UNSIGNED_INT, 0));
	}
}

static void _render_node_shadow(DynamicNode *node)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	DynamicNode *child;
	if(node->Flags & SCENE_FLAG_DISABLE)
	{
		return;
	}

	if(node->Flags & SCENE_FLAG_SHADOW)
	{
		shadow_volume_generate(renderer->Scene->Lighting->PointLights.Position[0], node->PositionMatrix, node->ShadowID);
	}

	for(child = node->Children; child; child = child->Next)
	{
		_render_node_shadow(child);
	}
}

/* SHADOW MAP */
static void _render_shadow_map_instances(u32 offset, u32 count, mat4 *m)
{
	GL_CHECK(glUniformMatrix4fv(_uniform_position, (GLsizei)count, GL_FALSE, (const GLfloat *)m));
	GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, (GLint)_smo[offset], (GLsizei)_smc[offset], (GLsizei)count));
}

void scene_render_shadow_map(mat4 projection)
{
	u32 i, start, count;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glUniformMatrix4fv(renderer->UniformShadowMap_Perspective, 1, GL_FALSE, &projection[0][0]));
	_uniform_position = renderer->UniformShadowMap_PositionMatrix;

	start = 0;
	count = 1;
	for(i = 1; i < _si; ++i)
	{
		if(_smo[_ss[i - 1]] != _smo[_ss[i]])
		{
			_render_shadow_map_instances(_ss[start], count, &_spm[start]);
			start = i;
			count = 0;
		}

		++count;
	}

	if(_si)
	{
		_render_shadow_map_instances(_ss[start], count, &_spm[start]);
	}
}

/* RENDER SELECTION DYNAMIC */
static void _render_selection_instances(u32 offset, u32 count, mat4 *m, float *id)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glUniform1fv(renderer->UniformSelection_SelectionID, (GLsizei)count, id));
	GL_CHECK(glUniformMatrix4fv(_uniform_position, (GLsizei)count, GL_FALSE, (const GLfloat *)m));
	GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, (GLint)_smo[offset], (GLsizei)_smc[offset], (GLsizei)count));
}

void scene_render_selection(mat4 projection)
{
	u32 i, start, count;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glUniformMatrix4fv(renderer->UniformSelection_Perspective, 1, GL_FALSE, &projection[0][0]));
	_uniform_position = renderer->UniformSelection_PositionMatrix;

	start = 0;
	count = 1;
	for(i = 1; i < _si; ++i)
	{
		if(_smo[_ss[i - 1]] != _smo[_ss[i]])
		{
			_render_selection_instances(_ss[start], count, &_spm[start], &_sid[start]);
			start = i;
			count = 0;
		}

		++count;
	}

	if(_si)
	{
		_render_selection_instances(_ss[start], count, &_spm[start], &_sid[start]);
	}
}

/* SELECTION POSITION */
static void _render_selection_position_instances(u32 offset, u32 count, mat4 *m)
{
	GL_CHECK(glUniformMatrix4fv(_uniform_position, (GLsizei)count, GL_FALSE, (const GLfloat *)m));
	GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, (GLint)_smo[offset], (GLsizei)_smc[offset], (GLsizei)count));
}

void scene_render_selection_position(mat4 projection)
{
	u32 i, start, count;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glUniformMatrix4fv(renderer->UniformSelectionPos_Perspective, 1, GL_FALSE, &projection[0][0]));
	_uniform_position = renderer->UniformSelectionPos_PositionMatrix;

	start = 0;
	count = 1;
	for(i = 1; i < _si; ++i)
	{
		if(_smo[_ss[i - 1]] != _smo[_ss[i]])
		{
			_render_selection_position_instances(_ss[start], count, &_spm[start]);
			start = i;
			count = 0;
		}

		++count;
	}

	if(_si)
	{
		_render_selection_position_instances(_ss[start], count, &_spm[start]);
	}
}

/* RESET */
void node_matrix(DynamicNode *node)
{
	mat4_identity(node->ModelMatrix);
}

void node_reset(DynamicNode *node, float x, float y, float z)
{
	mat4_translate(node->ModelMatrix, x, y, z);
}

void node_reset_v3(DynamicNode *node, vec3 p)
{
	mat4_translate(node->ModelMatrix, p[X], p[Y], p[Z]);
}

/* TRANSLATE */
void node_translate_x(DynamicNode *node, float x)
{
	mat4_translate_in_place(node->ModelMatrix, x, 0.0f, 0.0f);
}

void node_translate_y(DynamicNode *node, float y)
{
	mat4_translate_in_place(node->ModelMatrix, 0.0f, y, 0.0f);
}

void node_translate_z(DynamicNode *node, float z)
{
	mat4_translate_in_place(node->ModelMatrix, 0.0f, 0.0f, z);
}

void node_translate_xy(DynamicNode *node, float x, float y)
{
	mat4_translate_in_place(node->ModelMatrix, x, y, 0.0f);
}

void node_translate_xz(DynamicNode *node, float x, float z)
{
	mat4_translate_in_place(node->ModelMatrix, x, 0.0f, z);
}

void node_translate_yz(DynamicNode *node, float y, float z)
{
	mat4_translate_in_place(node->ModelMatrix, 0.0f, y, z);
}

void node_translate(DynamicNode *node, float x, float y, float z)
{
	mat4_translate_in_place(node->ModelMatrix, x, y, z);
}

void node_translate_v3(DynamicNode *node, vec3 p)
{
	mat4_translate_in_place(node->ModelMatrix, p[X], p[Y], p[Z]);
}

/* ROTATE */
void node_rotate_x(DynamicNode *node, float r)
{
	mat4_rotate_x(node->ModelMatrix, node->ModelMatrix, r);
}

void node_rotate_y(DynamicNode *node, float r)
{
	mat4_rotate_y(node->ModelMatrix, node->ModelMatrix, r);
}

void node_rotate_z(DynamicNode *node, float r)
{
	mat4_rotate_z(node->ModelMatrix, node->ModelMatrix, r);
}

void node_rotate_1(DynamicNode *node, int a, float ra)
{
	switch(a)
	{
		case X:
			node_rotate_x(node, ra);
			break;

		case Y:
			node_rotate_y(node, ra);
			break;

		case Z:
			node_rotate_z(node, ra);
			break;
	}
}

void node_rotate_2(DynamicNode *node, int a, float ra, int b, float rb)
{
	node_rotate_1(node, a, ra);
	node_rotate_1(node, b, rb);
}

void node_rotate_3(DynamicNode *node, int a, float ra, int b, float rb, int c, float rc)
{
	node_rotate_1(node, a, ra);
	node_rotate_1(node, b, rb);
	node_rotate_1(node, c, rc);
}

/* SCALE */
void node_scale_x(DynamicNode *node, float x)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, 1.0f, 1.0f, x);
}

void node_scale_y(DynamicNode *node, float y)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, 1.0f, y, 1.0f);
}

void node_scale_z(DynamicNode *node, float z)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, 1.0f, 1.0f, z);
}

void node_scale_xz(DynamicNode *node, float x, float z)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, x, 1.0f, z);
}

void node_scale_xy(DynamicNode *node, float x, float y)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, x, y, 1.0f);
}

void node_scale_yz(DynamicNode *node, float y, float z)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, 1.0f, y, z);
}

void node_scale(DynamicNode *node, float x, float y, float z)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, x, y, z);
}

void node_scale_uniform(DynamicNode *node, float x)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, x, x, x);
}

void node_scale_v3(DynamicNode *node, vec3 p)
{
	mat4_scale(node->ModelMatrix, node->ModelMatrix, p[X], p[Y], p[Z]);
}

/* MUL */
void node_mul(DynamicNode *node, mat4 m)
{
	mat4_mul(node->ModelMatrix, node->ModelMatrix, m);
}

/* CENTER */
void node_center(MeshCollection *mc, DynamicNode *node)
{
	vec3 center;
	u32 mid = node->MeshID;
	vec3_sub(center, mc->AABBs[mid].Max, mc->AABBs[mid].Min);
	vec3_add(center, center, mc->AABBs[mid].Min);
	node_reset(node, -center[X] / 2.0f, -center[Y] / 2.0f, -center[Z] / 2.0f);
}

