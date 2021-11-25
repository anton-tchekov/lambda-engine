#ifndef __FONT_H__
#define __FONT_H__

#include "aff.h"
#include "types.h"
#include "vector.h"
#include "opengl.h"

typedef struct FONT_COLLECTION
{
	GLsizei TextureSize;
	u32 NumFonts;
	Font *Fonts;
} FontCollection;

int renderer_font_init(void);
int renderer_font_use(void);

void renderer_font_projection(mat4 projection);
void renderer_font_window(void);

void font_collection_load_aff(const AFF *restrict aff, FontCollection *restrict fc);
int font_collection_load_aff_memory(u8 *buffer, FontCollection *fc);

void font_collection_use(FontCollection *fc);
void font_collection_destroy(FontCollection *fc);

i32 font_render_string(i32 x, i32 y, const char *restrict s, u32 len);
i32 font_width_string(const char *restrict s, u32 len);
i32 font_height_string(const char *restrict s, u32 len);

#endif

