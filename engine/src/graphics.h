#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "vector.h"
#include "types.h"

int key_pressed(int key);

int graphics_init(const char *title, int min_w, int min_h);
void graphics_destroy(void);

void graphics_screenshot(void);

void graphics_vr_mode(int vr);
int graphics_get_vr_mode(void);

void graphics_wireframe(int i);
int graphics_get_wireframe(void);

void graphics_fullscreen(void);
void graphics_windowed(void);
void graphics_mode(u32 n);
int graphics_get_mode(void);

void graphics_event_mode_enable(void);
void graphics_event_mode_disable(void);

void graphics_vsync_enable(void);
void graphics_vsync_disable(void);
void graphics_vsync(u32 n);
int graphics_get_vsync(void);

void graphics_mouse_lock(void);
void graphics_mouse_unlock(void);

void graphics_aa_method(int method);
int graphics_get_aa_method(void);

void graphics_get_selpos_v3(vec3 pos);

void graphics_get_sel(u32 *x, u32 *y, u32 *z);
void graphics_get_selpos(float *x, float *y, float *z);

u32 graphics_vbo_binds(void);
u32 graphics_vao_binds(void);
u32 graphics_texture_binds(void);
u32 graphics_fbo_binds(void);
u32 graphics_draw_calls(void);
u32 graphics_vertices(void);

#endif

