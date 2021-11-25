#include "billboard.h"
#include "internal.h"
#include "debug.h"
#include "uniform.h"
#include "strings.h"
#include "shader.h"
#include "shaders.h"
#include "condition.h"
#include "color.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>

/* Screen */
static Rect2D *_get_area(Screen *screen, int area)
{
	switch(area)
	{
		case 0:
			return &screen->SourceDiffuse;

		case 1:
			return &screen->SourceEmissive;

		case 2:
			return &screen->SourceSelection;
	}

	return NULL;
}

void screen_preinit(void)
{
	/* Black Square */
	r2d_color8(COLOR8.BLACK);
	r2d_rect(CORNER_BLACK_X, CORNER_BLACK_Y, CORNER_BLACK_W, CORNER_BLACK_H);

	/* White Square */
	r2d_color8(COLOR8.WHITE);
	r2d_rect(CORNER_WHITE_X, CORNER_WHITE_Y, CORNER_WHITE_W, CORNER_WHITE_H);
}

/* Render Rectangle on Screen */
void screen_rect(Screen *screen, int area, int x, int y, int w, int h)
{
	Rect2D *rect = _get_area(screen, area);
	r2d_rect((*rect)[0] + x, (*rect)[1] + y, w, h);
}

void screen_rect_r(Screen *screen, int area, Rect2D r)
{
	screen_rect(screen, area, r[0], r[1], r[2], r[2]);
}

/* Render Sprite on Screen */
void screen_sprite(Screen *screen, int area, int x0, int y0, int w0, int h0,
		int x1, int y1, int w1, int h1, int layer)
{
	Rect2D *rect = _get_area(screen, area);
	r2d_sprite((*rect)[0] + x0, (*rect)[1] + y0, w0, h0, x1, y1, w1, h1, layer);
}

void screen_sprite_r(Screen *screen, int area, Rect2D r0, Rect2D r1, int layer)
{
	screen_sprite(screen, area, r0[0], r0[1], r0[2], r0[3],
			r1[0], r1[1], r1[2], r1[3], layer);
}

/* Render String on Screen */
void screen_string(Screen *screen, int area, int x, int y, const char *s, u32 len)
{
	Rect2D *rect = _get_area(screen, area);
	r2d_string((*rect)[0] + x, (*rect)[1] + y, s, len);
}

/* Render */
void screen_render(Screen *screen)
{
	if(!screen->ConditionID || condition_get(screen->ConditionID))
	{
		billboard_add(screen->SourceDiffuse, screen->SourceEmissive, screen->Position, screen->ModelMatrix,
				screen->Width, screen->Height, screen->SelectionID, screen->SourceSelection);
	}
}

/* Pixel Transfer */
void screen_upload(Screen *screen, int area, int x, int y, int w, int h, u8 *pixels)
{
	Rect2D *rect = _get_area(screen, area);
	billboard_upload((*rect)[0] + x, (*rect)[1] + y, w, h, pixels);
}

void screen_download(Screen *screen, int area, int x, int y, int w, int h, u8 *pixels)
{
	Rect2D *rect = _get_area(screen, area);
	billboard_download((*rect)[0] + x, (*rect)[1] + y, w, h, pixels);
}

/* Billboard */
#define STRIDE 32

static u32 pack_gl_int_2_10_10_10_rev(u32 x, u32 y, u32 z, u32 w)
{
	return ((w & 0x03) << 30) | ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10) | (x & 0x3FF);
}

static u32 fp_normalize(float normal)
{
	i32 v = (i32)(normal * 511.0f);
	return (u32)(((v < 0) << 9) | (v & 0x1FF));
}

static void write_u32(u8 *dst, u32 val)
{
	dst[0] = (u8)val;
	dst[1] = (u8)(val >> 8);
	dst[2] = (u8)(val >> 16);
	dst[3] = (u8)(val >> 24);
}

static void write_i16(u8 *dst, i16 val)
{
	dst[0] = (u8)val;
	dst[1] = (u8)(val >> 8);
}

static void write_u16(u8 *dst, u16 val)
{
	dst[0] = (u8)val;
	dst[1] = (u8)(val >> 8);
}

static void write_float(u8 *dst, float val)
{
	u32 i;
	memcpy(&i, &val, 4);
	dst[0] = (u8)i;
	dst[1] = (u8)(i >> 8);
	dst[2] = (u8)(i >> 16);
	dst[3] = (u8)(i >> 24);
}

int billboard_init(GLsizei max_billboards)
{
	Billboard *renderer = &GameEngine.Graphics.Billboard;
	GLint i;
	GLsizei size_v, size_i;
	log_debug("Initializing billboard renderer");
	renderer->MaxBillboards = max_billboards;

	size_v = 4 * STRIDE * max_billboards;
	if(!(renderer->Vertices = malloc((size_t)size_v)))
	{
		return 1;
	}

	size_i = 6 * 2 * max_billboards;
	if(!(renderer->Indices = malloc((size_t)size_i)))
	{
		free(renderer->Vertices);
		return 1;
	}

	if(!(renderer->Shader = shader_load_string_vf(billboard_vs, billboard_fs)))
	{
		free(renderer->Vertices);
		free(renderer->Indices);
		return 1;
	}

	if(!(renderer->ShaderSelection = shader_load_string_vf(billboard_sel_vs, billboard_sel_fs)))
	{
		free(renderer->Vertices);
		free(renderer->Indices);
		return 1;
	}

	GL_CHECK(glUseProgram(renderer->Shader));
	renderer->Uniform_Perspective = uniform_get(renderer->Shader, s_uPerspective);
	renderer->Uniform_PL_Position = uniform_get(renderer->Shader, s_uPL_Position);
	renderer->Uniform_PL_Color = uniform_get(renderer->Shader, s_uPL_Color);
	renderer->Uniform_PL_Constant = uniform_get(renderer->Shader, s_uPL_Constant);
	renderer->Uniform_PL_Linear = uniform_get(renderer->Shader, s_uPL_Linear);
	renderer->Uniform_PL_Quadratic = uniform_get(renderer->Shader, s_uPL_Quadratic);
	renderer->Uniform_Shadow = uniform_get(renderer->Shader, s_uShadow);
	renderer->Uniform_Dark = uniform_get(renderer->Shader, s_uDark);

	i = uniform_get(renderer->Shader, s_uTexture);
	GL_CHECK(glUniform1i(i, TU_BILLBOARD));

	GL_CHECK(glUseProgram(renderer->ShaderSelection));
	renderer->Uniform_SelectionPerspective = uniform_get(renderer->ShaderSelection, s_uPerspective);

	i = uniform_get(renderer->ShaderSelection, s_uTexture);
	GL_CHECK(glUniform1i(i, TU_BILLBOARD));

	GL_CHECK(glGenVertexArrays(1, &renderer->VAO));
	GL_CHECK(glGenBuffers(2, renderer->VBO));
	GL_CHECK(glBindVertexArray(renderer->VAO));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO[0]));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, size_v, NULL, GL_STREAM_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, NULL, GL_STREAM_DRAW));

	/* Vertex Specification */
	/* Position */
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, (void *)0));

	/* Normal */
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 4, GL_INT_2_10_10_10_REV, GL_TRUE, STRIDE, (void *)12));

	/* Diffuse */
	GL_CHECK(glEnableVertexAttribArray(2));
	GL_CHECK(glVertexAttribPointer(2, 2, GL_SHORT, GL_FALSE, STRIDE, (void *)16));

	/* Emissive */
	GL_CHECK(glEnableVertexAttribArray(3));
	GL_CHECK(glVertexAttribPointer(3, 2, GL_SHORT, GL_FALSE, STRIDE, (void *)20));

	/* Selection */
	GL_CHECK(glEnableVertexAttribArray(4));
	GL_CHECK(glVertexAttribPointer(4, 4, GL_UNSIGNED_SHORT, GL_TRUE, STRIDE, (void *)24));

	GL_CHECK(glGenFramebuffers(1, &renderer->FBO));
	GL_CHECK(glGenTextures(1, &renderer->Texture));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->FBO));

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_BIND));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderer->Texture));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, BILLBOARD_TEXTURE_SIZE, BILLBOARD_TEXTURE_SIZE,
			0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->Texture, 0));
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return 1;
	}

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_BILLBOARD));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderer->Texture));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	mat4_ortho(renderer->Projection, 0.0, (float)BILLBOARD_TEXTURE_SIZE, 0.0, (float)BILLBOARD_TEXTURE_SIZE, -1.0, 1.0);
	return 0;
}

void billboard_prepare(void)
{
	Billboard *renderer = &GameEngine.Graphics.Billboard;
	renderer->BillboardIndex = 0;
	billboard_update();

	renderer->NumBillboards = renderer->BillboardIndex;
	if(renderer->BillboardIndex)
	{
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO[0]));
		GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->NumBillboards * 4 * STRIDE, renderer->Vertices));

		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
		GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, renderer->NumBillboards * 6 * 2, renderer->Indices));
	}
}

static void _vertex(u8 *vertices, vec3 position, u32 normal,
		i16 x0, i16 y0, i16 x1, i16 y1, u16 sel_id, u16 sel_x, u16 sel_y)
{
	write_float(vertices, position[0]);
	write_float(vertices + 4, position[1]);
	write_float(vertices + 8, position[2]);
	write_u32(vertices + 12, normal);
	write_i16(vertices + 16, x0);
	write_i16(vertices + 18, y0);
	write_i16(vertices + 20, x1);
	write_i16(vertices + 22, y1);

	write_u16(vertices + 24, sel_id);
	write_u16(vertices + 26, sel_x);
	write_u16(vertices + 28, sel_y);
	write_u16(vertices + 30, 0);
}

void billboard_add(Rect2D src0, Rect2D src1, vec3 center, mat4 model, float w, float h, int sel_id, Rect2D sel)
{
	vec3 positions[4] =
	{
		{ -0.5f,  0.5f, 0.0f },
		{  0.5f,  0.5f, 0.0f },
		{ -0.5f, -0.5f, 0.0f },
		{  0.5f, -0.5f, 0.0f }
	};

	Billboard *renderer = &GameEngine.Graphics.Billboard;
	vec3 normal, mul_positions[4];
	u32 packed_normal;
	GLsizei vertex_idx;
	u8 *vertices, *indices;
	i16 x00, y00, x10, y10, x01, y01, x11, y11;

	if(renderer->BillboardIndex >= renderer->MaxBillboards - 1)
	{
		return;
	}

	x00 = (i16)(src0[X] + src0[2]);
	y00 = src0[Y];
	x10 = src0[X];
	y10 = (i16)(src0[Y] + src0[3]);

	x01 = (i16)(src1[X] + src1[2]);
	y01 = src1[Y];
	x11 = src1[X];
	y11 = (i16)(src1[Y] + src1[3]);

	vertex_idx = 4 * renderer->BillboardIndex;
	vertices = renderer->Vertices + vertex_idx * STRIDE;
	indices = renderer->Indices + renderer->BillboardIndex * 6 * 2;

	write_u16(indices, (u16)(vertex_idx));
	write_u16(indices + 2, (u16)(vertex_idx + 1));
	write_u16(indices + 4, (u16)(vertex_idx + 2));

	write_u16(indices + 6, (u16)(vertex_idx + 1));
	write_u16(indices + 8, (u16)(vertex_idx + 3));
	write_u16(indices + 10, (u16)(vertex_idx + 2));

	/* Scale */
	positions[0][X] = positions[0][X] * w;
	positions[1][X] = positions[1][X] * w;
	positions[2][X] = positions[2][X] * w;
	positions[3][X] = positions[3][X] * w;

	positions[0][Y] = positions[0][Y] * h;
	positions[1][Y] = positions[1][Y] * h;
	positions[2][Y] = positions[2][Y] * h;
	positions[3][Y] = positions[3][Y] * h;

	/* Transform */
	mat4_mul_vec3(mul_positions[0], model, positions[0]);
	mat4_mul_vec3(mul_positions[1], model, positions[1]);
	mat4_mul_vec3(mul_positions[2], model, positions[2]);
	mat4_mul_vec3(mul_positions[3], model, positions[3]);

	/* Translate */
	vec3_add(mul_positions[0], mul_positions[0], center);
	vec3_add(mul_positions[1], mul_positions[1], center);
	vec3_add(mul_positions[2], mul_positions[2], center);
	vec3_add(mul_positions[3], mul_positions[3], center);

	tri_norm(normal, mul_positions[0], mul_positions[1], mul_positions[2]);
	packed_normal = pack_gl_int_2_10_10_10_rev(fp_normalize(normal[0]), fp_normalize(normal[1]), fp_normalize(normal[2]), 0);

	/* Vertices */
	u16 x1 = (u16)(sel[0] + sel[2]), y1 = (u16)(sel[1] + sel[3]);
	_vertex(vertices, mul_positions[0], packed_normal, x00, y00, x01, y01, (u16)sel_id, x1, (u16)sel[1]);
	_vertex(vertices + STRIDE, mul_positions[1], packed_normal, x10, y00, x11, y01, (u16)sel_id, (u16)sel[0], (u16)sel[1]);
	_vertex(vertices + 2 * STRIDE, mul_positions[2], packed_normal, x00, y10, x01, y11, (u16)sel_id, x1, y1);
	_vertex(vertices + 3 * STRIDE, mul_positions[3], packed_normal, x10, y10, x11, y11, (u16)sel_id, (u16)sel[0], y1);

	++renderer->BillboardIndex;
}

void billboard_render(mat4 perspective, float shadow)
{
	Lighting *lighting = GameEngine.Graphics.Renderer3D.Scene->Lighting;
	Billboard *renderer = &GameEngine.Graphics.Billboard;

	if(!renderer->NumBillboards)
	{
		return;
	}

	GL_CHECK(glEnable(GL_CULL_FACE));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glUseProgram(renderer->Shader));
	GL_CHECK(glUniformMatrix4fv(renderer->Uniform_Perspective, 1, GL_FALSE, &perspective[0][0]));

	GL_CHECK(glUniform3fv(
		renderer->Uniform_PL_Position,
		NUM_POINT_LIGHTS, &lighting->PointLights.Position[0][0]));

	GL_CHECK(glUniform3fv(
		renderer->Uniform_PL_Color,
		NUM_POINT_LIGHTS, &lighting->PointLights.Color[0][0]));

	GL_CHECK(glUniform1fv(
		renderer->Uniform_PL_Constant,
		NUM_POINT_LIGHTS, lighting->PointLights.Constant));

	GL_CHECK(glUniform1fv(
		renderer->Uniform_PL_Linear,
		NUM_POINT_LIGHTS, lighting->PointLights.Linear));

	GL_CHECK(glUniform1fv(
		renderer->Uniform_PL_Quadratic,
		NUM_POINT_LIGHTS, lighting->PointLights.Quadratic));

	GL_CHECK(glUniform1f(
		renderer->Uniform_Shadow,
		shadow));

	GL_CHECK(glUniform1f(renderer->Uniform_Dark, GameEngine.Graphics.Renderer3D.Dark));

	GL_CHECK(glBindVertexArray(renderer->VAO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
	GL_CHECK(glDrawElements(GL_TRIANGLES, 6 * renderer->NumBillboards, GL_UNSIGNED_SHORT, 0));
}

void billboard_selection_render(mat4 perspective)
{
	Billboard *renderer = &GameEngine.Graphics.Billboard;
	if(!renderer->NumBillboards)
	{
		return;
	}

	GL_CHECK(glEnable(GL_CULL_FACE));
	GL_CHECK(glUseProgram(renderer->ShaderSelection));
	GL_CHECK(glUniformMatrix4fv(renderer->Uniform_SelectionPerspective, 1, GL_FALSE, &perspective[0][0]));

	GL_CHECK(glBindVertexArray(renderer->VAO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
	GL_CHECK(glDrawElements(GL_TRIANGLES, 6 * renderer->NumBillboards, GL_UNSIGNED_SHORT, 0));
}

void billboard_destroy(void)
{
	Billboard *renderer = &GameEngine.Graphics.Billboard;
	log_debug("Destroying billboard renderer");
	free(renderer->Vertices);
	free(renderer->Indices);
	GL_CHECK(glDeleteProgram(renderer->Shader));
	GL_CHECK(glDeleteFramebuffers(1, &renderer->FBO));
	GL_CHECK(glDeleteTextures(1, &renderer->Texture));
}

