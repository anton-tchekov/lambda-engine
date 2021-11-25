#ifndef __RENDERER2D_H__
#define __RENDERER2D_H__

#include "opengl.h"
#include "color.h"
#include "texture.h"
#include "font.h"
#include "defines.h"

typedef i16 Rect2D[4];

int renderer2d_init(GLsizei max_sprites);
void renderer2d_render(void);
void renderer2d_billboard_render(void);
void renderer2d_prepare(void);
void renderer2d_bind_texture(Texture *texture);
void renderer2d_bind_font_collection(FontCollection *fc);
void renderer2d_dimensions(mat4 projection, int width, int height);
void renderer2d_destroy(void);
void renderer2d_fill_square(int x, int y, int w, int h);
void renderer2d_sprite(const Rect2D dst, const Rect2D src, int layer);

/* Pixel Transfer */
void billboard_upload(int x, int y, int w, int h, u8 *pixels);
void billboard_download(int x, int y, int w, int h, u8 *pixels);

/* User implemented */
void sprite_update(void);
void billboard_clear_update(void);
void billboard_sprite_update(void);
void billboard_pixel_transfer(void);

/* Shorthands, should be optimized by LTO */
void r2d_color8(const ColorRGBA8 color);
void r2d_maxwidth(u32 w);
void r2d_lineheight(u32 h);
void r2d_color(u8 r, u8 g, u8 b, u8 a);
void r2d_font(u32 font_id, u32 size_id);
int r2d_width(const char *s, u32 len);
void r2d_string(int x, int y, const char *s, u32 len);
void r2d_rect(int x0, int y0, int w0, int h0);
void r2d_rect_r(Rect2D r);
void r2d_quad(
	int x0, int y0,
	int x1, int y1,
	int x2, int y2,
	int x3, int y3);
void r2d_sprite(int x0, int y0, int w0, int h0,
		int x1, int y1, int w1, int h1, int layer);
void r2d_sprite_r(const Rect2D r0, const Rect2D r1, int layer);

#endif

