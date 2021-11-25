#include "renderer2d.h"
#include "internal.h"
#include "log.h"
#include "debug.h"
#include "shader.h"
#include "shaders.h"
#include "uniform.h"
#include "font.h"
#include "strings.h"
#include <stdlib.h>
#include <string.h>

static Rect2D _fill;

#define BYTES_PER_INDEX  (GLsizei)(sizeof(u16))
#define BYTES_PER_VERTEX (GLsizei)(5 * sizeof(i16) + 4 * sizeof(u8))

static u8 *_vertex(u8 *p, i16 x, i16 y, i16 z, i16 u, i16 v, ColorRGBA8 color);

static int _renderer2d_shaders(void)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	GLuint vertex, fragment;
	log_debug("Loading 2D renderer shaders");
	if(!(vertex = shader_compile_string(renderer2d_vs, GL_VERTEX_SHADER)))
	{
		return 1;
	}

	if(!(fragment = shader_compile_string(renderer2d_fs, GL_FRAGMENT_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		return 1;
	}

	renderer->Shader = shader_link_vf(vertex, fragment);
	GL_CHECK(glDeleteShader(vertex));
	GL_CHECK(glDeleteShader(fragment));
	return 0;
}

int renderer2d_init(GLsizei max_sprites)
{
	GLint i;
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	log_debug("Initializing 2D renderer");
	if(_renderer2d_shaders())
	{
		return 1;
	}

	GL_CHECK(glUseProgram(renderer->Shader));
	renderer->UniformTextureSize = uniform_get(renderer->Shader, s_uTextureSize);
	renderer->UniformProjection = uniform_get(renderer->Shader, s_uProjection);
	i = uniform_get(renderer->Shader, s_uTexture);
	GL_CHECK(glUniform1i(i, TU_SPRITE));

	renderer->MaxTotalSprites = max_sprites;
	if(!(renderer->Vertices = malloc((size_t)(max_sprites * 4 * BYTES_PER_VERTEX))))
	{
		log_error("Failed to allocate memory for 2D vertices");
		return 1;
	}

	if(!(renderer->Indices = malloc((size_t)(max_sprites * 6 * BYTES_PER_INDEX))))
	{
		log_error("Failed to allocate memory for 2D indices");
		return 1;
	}

	GL_CHECK(glGenVertexArrays(1, &renderer->VAO));
	GL_CHECK(glBindVertexArray(renderer->VAO));

	GL_CHECK(glGenBuffers(2, renderer->VBO));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO[0]));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, max_sprites * 4 * BYTES_PER_VERTEX, NULL, GL_STREAM_DRAW));

	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, BYTES_PER_VERTEX, (void *)0));

	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 2, GL_SHORT, GL_FALSE, BYTES_PER_VERTEX, (void *)(3 * sizeof(i16))));

	GL_CHECK(glEnableVertexAttribArray(2));
	GL_CHECK(glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, BYTES_PER_VERTEX, (void *)(5 * sizeof(i16))));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_sprites * 6 * BYTES_PER_INDEX, NULL, GL_STREAM_DRAW));
	return 0;
}

void renderer2d_fill_square(int x, int y, int w, int h)
{
	_fill[0] = (i16)x;
	_fill[1] = (i16)y;
	_fill[2] = (i16)w;
	_fill[3] = (i16)h;
}

void renderer2d_bind_texture(Texture *texture)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_SPRITE));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, texture->TextureID));
	renderer->BoundTexture = texture;
	renderer->TextureDirty = 1;
}

void renderer2d_bind_font_collection(FontCollection *fc)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	renderer->BoundFC = fc;
}

void renderer2d_prepare(void)
{
	int i;
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	i = 0;
	renderer->NumTotalSprites = 0;

	sprite_update();
	renderer->NumSprites = renderer->NumTotalSprites;
	i = renderer->NumTotalSprites;

	billboard_clear_update();
	renderer->BillboardNumClears = renderer->NumTotalSprites - i;
	i = renderer->NumTotalSprites;

	billboard_sprite_update();
	renderer->BillboardNumSprites = renderer->NumTotalSprites - i;
	i = renderer->NumTotalSprites;

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO[0]));
	GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * BYTES_PER_VERTEX * renderer->NumTotalSprites, renderer->Vertices));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
	GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 6 * BYTES_PER_INDEX * renderer->NumTotalSprites, renderer->Indices));
}

void renderer2d_billboard_render(void)
{
	int i;
	Billboard *billboard = &GameEngine.Graphics.Billboard;
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;

	if(!renderer->BillboardNumSprites && !renderer->BillboardNumClears)
	{
		return;
	}

	GL_CHECK(glBindVertexArray(renderer->VAO));
	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glDisable(GL_CULL_FACE));
	GL_CHECK(glDisable(GL_DEPTH_TEST));

	GL_CHECK(glViewport(0, 0, BILLBOARD_TEXTURE_SIZE, BILLBOARD_TEXTURE_SIZE));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, billboard->FBO));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));
	GL_CHECK(glBlendFunc(GL_ONE, GL_ZERO));

	i = 6 * (renderer->NumSprites) * BYTES_PER_INDEX;
	if(renderer->BillboardNumClears)
	{
		GL_CHECK(glUseProgram(renderer->Shader));
		GL_CHECK(glUniformMatrix4fv(renderer->UniformProjection, 1, GL_FALSE, &billboard->Projection[0][0]));
		if(renderer->TextureDirty)
		{
			GL_CHECK(glUniform1f(GameEngine.Graphics.Renderer2D.UniformTextureSize, (float)renderer->BoundTexture->Size));
			renderer->TextureDirty = 0;
		}

		GL_CHECK(glDrawElements(GL_TRIANGLES, (GLsizei)(6 * renderer->BillboardNumClears), GL_UNSIGNED_SHORT,
			(void *)(ptrdiff_t)i));
	}

	GL_CHECK(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

	i += 6 * renderer->BillboardNumClears * BYTES_PER_INDEX;
	if(renderer->BillboardNumSprites)
	{
		if(!renderer->BillboardNumClears)
		{
			GL_CHECK(glUseProgram(renderer->Shader));
			GL_CHECK(glUniformMatrix4fv(renderer->UniformProjection, 1, GL_FALSE, &billboard->Projection[0][0]));
		}

		if(renderer->TextureDirty)
		{
			GL_CHECK(glUniform1f(GameEngine.Graphics.Renderer2D.UniformTextureSize, (float)renderer->BoundTexture->Size));
			renderer->TextureDirty = 0;
		}

		GL_CHECK(glDrawElements(GL_TRIANGLES, (GLsizei)(6 * renderer->BillboardNumSprites), GL_UNSIGNED_SHORT,
			(void *)(ptrdiff_t)i));
	}

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_BILLBOARD));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, billboard->Texture));


	billboard_pixel_transfer();

	GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void billboard_upload(int x, int y, int w, int h, u8 *pixels)
{
	GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0,
			(GLint)x, (GLint)y, (GLsizei)w, (GLsizei)h, GL_BGRA, GL_UNSIGNED_BYTE, pixels));
}

void billboard_download(int x, int y, int w, int h, u8 *pixels)
{
	GL_CHECK(glReadPixels((GLint)x, (GLint)y, (GLsizei)w, (GLsizei)h,
			GL_BGRA, GL_UNSIGNED_BYTE, pixels));
}

void renderer2d_render(void)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	GL_CHECK(glBindVertexArray(renderer->VAO));
	GL_CHECK(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glDisable(GL_CULL_FACE));
	GL_CHECK(glDisable(GL_DEPTH_TEST));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->VBO[1]));

	if(renderer->NumSprites)
	{
		GL_CHECK(glUseProgram(renderer->Shader));
		GL_CHECK(glUniformMatrix4fv(renderer->UniformProjection, 1, GL_FALSE, &renderer->ProjectionWindow[0][0]));
		if(renderer->TextureDirty)
		{
			GL_CHECK(glUniform1f(GameEngine.Graphics.Renderer2D.UniformTextureSize, (float)renderer->BoundTexture->Size));
			renderer->TextureDirty = 0;
		}

		GL_CHECK(glDrawElements(GL_TRIANGLES, 6 * renderer->NumSprites, GL_UNSIGNED_SHORT, (void *)0));
	}
}

static u8 *_vertex(u8 *p, i16 x, i16 y, i16 z, i16 u, i16 v, ColorRGBA8 color)
{
	*p++ = (u8)u;
	*p++ = (u8)(u >> 8);

	*p++ = (u8)v;
	*p++ = (u8)(v >> 8);

	*p++ = (u8)z;
	*p++ = (u8)(z >> 8);

	*p++ = (u8)x;
	*p++ = (u8)(x >> 8);

	*p++ = (u8)y;
	*p++ = (u8)(y >> 8);

	*p++ = color[0];
	*p++ = color[1];
	*p++ = color[2];
	*p++ = color[3];
	return p;
}

void renderer2d_destroy(void)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	GL_CHECK(glDeleteVertexArrays(1, &renderer->VAO));
	GL_CHECK(glDeleteBuffers(2, renderer->VBO));
	free(renderer->Vertices);
	free(renderer->Indices);
	GL_CHECK(glDeleteProgram(renderer->Shader));
}

void renderer2d_dimensions(mat4 projection, int width, int height)
{
	mat4_ortho(projection, 0.0, (float)width, (float)height, 0.0, -1.0, 1.0);
}

void renderer2d_sprite(const Rect2D dst, const Rect2D src, int layer)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	i16 x0, x1, y0, y1, u0, u1, v0, v1;
	u16 i;
	u8 *p;
	u16 *q;
	if(renderer->NumTotalSprites >= renderer->MaxTotalSprites - 1)
	{
		return;
	}

	x0 = dst[0];
	x1 = (i16)(dst[0] + dst[2]);

	y0 = dst[1];
	y1 = (i16)(dst[1] + dst[3]);

	u0 = src[0];
	u1 = (i16)(src[0] + src[2]);

	v0 = src[1];
	v1 = (i16)(src[1] + src[3]);

	q = renderer->Indices + 6 * renderer->NumTotalSprites;
	p = renderer->Vertices + 4 * BYTES_PER_VERTEX * renderer->NumTotalSprites;
	p = _vertex(p, x0, y0, (i16)layer, u0, v0, renderer->Color);
	p = _vertex(p, x1, y0, (i16)layer, u1, v0, renderer->Color);
	p = _vertex(p, x0, y1, (i16)layer, u0, v1, renderer->Color);
	_vertex(p, x1, y1, (i16)layer, u1, v1, renderer->Color);

	i = (u16)(4 * renderer->NumTotalSprites);
	q[0] = i + 0;
	q[1] = i + 1;
	q[2] = i + 2;
	q[3] = i + 1;
	q[4] = i + 2;
	q[5] = i + 3;

	++renderer->NumTotalSprites;
}

static void renderer2d_quad(
	int x0, int y0, int x1, int y1,
	int x2, int y2, int x3, int y3, const Rect2D src, int layer)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	i16 u0, u1, v0, v1;
	u16 i;
	u8 *p;
	u16 *q;
	if(renderer->NumTotalSprites >= renderer->MaxTotalSprites - 1)
	{
		return;
	}

	u0 = src[0];
	u1 = (i16)(src[0] + src[2]);

	v0 = src[1];
	v1 = (i16)(src[1] + src[3]);

	q = renderer->Indices + 6 * renderer->NumTotalSprites;
	p = renderer->Vertices + 4 * BYTES_PER_VERTEX * renderer->NumTotalSprites;
	p = _vertex(p, (i16)x0, (i16)y0, (i16)layer, u0, v0, renderer->Color);
	p = _vertex(p, (i16)x1, (i16)y1, (i16)layer, u1, v0, renderer->Color);
	p = _vertex(p, (i16)x2, (i16)y2, (i16)layer, u0, v1, renderer->Color);
	_vertex(p, (i16)x3, (i16)y3, (i16)layer, u1, v1, renderer->Color);

	i = (u16)(4 * renderer->NumTotalSprites);
	q[0] = i + 0;
	q[1] = i + 1;
	q[2] = i + 2;
	q[3] = i + 1;
	q[4] = i + 2;
	q[5] = i + 3;

	++renderer->NumTotalSprites;
}

void r2d_color8(const ColorRGBA8 color)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	memcpy(renderer->Color, color, 4);
}

void r2d_maxwidth(u32 w)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	renderer->MaxWidth = w;
}

void r2d_lineheight(u32 h)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	renderer->LineHeight = h;
}

void r2d_color(u8 r, u8 g, u8 b, u8 a)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	renderer->Color[0] = r;
	renderer->Color[1] = g;
	renderer->Color[2] = b;
	renderer->Color[3] = a;
}

void r2d_font(u32 font_id, u32 size_id)
{
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	renderer->FontID = font_id;
	renderer->SizeID = size_id;
}

int r2d_width(const char *s, u32 len)
{
	return font_width_string(s, len);
}

void r2d_string(int x, int y, const char *s, u32 len)
{
	font_render_string(x, y, s, len);
}

void r2d_rect(int x0, int y0, int w0, int h0)
{
	Rect2D dst = { (i16)x0, (i16)y0, (i16)w0, (i16)h0 };
	renderer2d_sprite(dst, _fill, 0);
}

void r2d_rect_r(Rect2D r)
{
	renderer2d_sprite(r, _fill, 0);
}

void r2d_quad(
	int x0, int y0,
	int x1, int y1,
	int x2, int y2,
	int x3, int y3)
{
	renderer2d_quad(x0, y0, x1, y1, x2, y2, x3, y3, _fill, 0);
}

void r2d_sprite(int x0, int y0, int w0, int h0,
		int x1, int y1, int w1, int h1, int layer)
{
	Rect2D dst = { (i16)x0, (i16)y0, (i16)w0, (i16)h0 };
	Rect2D src = { (i16)x1, (i16)y1, (i16)w1, (i16)h1 };
	renderer2d_sprite(dst, src, layer);
}

void r2d_sprite_r(const Rect2D r0, const Rect2D r1, int layer)
{
	renderer2d_sprite(r0, r1, layer);
}