#ifndef __BILLBOARD_H__
#define __BILLBOARD_H__

#include "types.h"
#include "vector.h"
#include "opengl.h"
#include "renderer2d.h"

#define SOURCE_DIFFUSE   0
#define SOURCE_EMISSIVE  1
#define SOURCE_SELECTION 2

typedef struct SCREEN
{
	Rect2D SourceDiffuse, SourceEmissive, SourceSelection;
	vec3 Position;
	mat4 ModelMatrix;
	float Width, Height;
	int SelectionID;
	u32 ConditionID;
} Screen;

void screen_preinit(void);
void screen_rect(Screen *screen, int area, int x, int y, int w, int h);
void screen_rect_r(Screen *screen, int area, Rect2D r);
void screen_sprite(Screen *screen, int area, int x0, int y0, int w0, int h0,
		int x1, int y1, int w1, int h1, int layer);

void screen_sprite_r(Screen *screen, int area, Rect2D r0, Rect2D r1, int layer);
void screen_string(Screen *screen, int area, int x, int y, const char *s, u32 len);
void screen_render(Screen *screen);

/* Pixel Transfer */
void screen_upload(Screen *screen, int area, int x, int y, int w, int h, u8 *pixels);
void screen_download(Screen *screen, int area, int x, int y, int w, int h, u8 *pixels);

/* Billboard */
int billboard_init(GLsizei max_billboards);
void billboard_prepare(void);
void billboard_add(Rect2D src0, Rect2D src1, vec3 center, mat4 model, float w, float h, int sel_id, Rect2D sel);
void billboard_render(mat4 perspective, float shadow);
void billboard_selection_render(mat4 perspective);
void billboard_destroy(void);

/* User implemented */
void billboard_update(void);

#endif

