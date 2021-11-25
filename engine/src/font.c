#include "font.h"
#include "utf8.h"
#include "internal.h"
#include "renderer2d.h"
#include <stdlib.h>

#define BYTES_PER_VERTEX (GLsizei)(4 * sizeof(i16) + 4 * sizeof(u8))

int font_collection_load_aff_memory(u8 *buffer, FontCollection *fc)
{
	AFF aff;
	if(aff_load_memory(buffer, &aff))
	{
		return 1;
	}

	font_collection_load_aff(&aff, fc);
	return 0;
}

void font_collection_load_aff(const AFF *restrict aff, FontCollection *restrict fc)
{
	fc->TextureSize = (GLsizei)aff->TextureSize;
	fc->NumFonts = aff->NumFonts;
	fc->Fonts = aff->Fonts;
}

void font_collection_destroy(FontCollection *fc)
{
	u32 i, j;
	for(i = 0; i < fc->NumFonts; ++i)
	{
		for(j = 0; j < fc->Fonts[i].NumSizes; ++j)
		{
			free(fc->Fonts[i].Sizes[j].Characters);
		}

		free(fc->Fonts[i].Sizes);
	}

	free(fc->Fonts);
}

static int _find_char(const void *restrict key, const void *restrict elem)
{
	const FontChar *fc = (const FontChar *)elem;
	i32 c0 = *(const i32 *)key;
	i32 c1 = fc->Character;
	if(c0 < c1)
	{
		return -1;
	}

	if(c0 > c1)
	{
		return 1;
	}

	return 0;
}

i32 font_render_string(i32 x, i32 y, const char *restrict s, u32 len)
{
	static u32 breaks[64];
	Rect2D src, dst;
	u32 j, i, break_count = 0;
	i32 c, x0;
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	FontSize *size = &renderer->BoundFC->Fonts[renderer->FontID].Sizes[renderer->SizeID];
	FontChar *chr;
	x0 = x;

	if(renderer->MaxWidth)
	{
		/* Auto-Break */
		const char *p = s;
		u32 last_space = 0;
		i32 w = 0;
		for(i = 0; ; ++i)
		{
			p = utf8(p, &c);
			if(!c)
			{
				break;
			}

			if(c == ' ')
			{
				last_space = i;
			}

			chr = bsearch(&c, size->Characters, size->NumCharacters, sizeof(FontChar), _find_char);
			if(!chr)
			{
				continue;
			}

			w += chr->Advance;
			if(w >= (i32)renderer->MaxWidth)
			{
				w = 0;
				breaks[break_count++] = last_space;
			}
		}
	}

	for(j = 0, i = 0; i < len; ++i)
	{
		s = utf8(s, &c);
		if(!c)
		{
			break;
		}

		if(j < break_count && breaks[j] == i)
		{
			++j;
			y += (i32)renderer->LineHeight;
			x = x0;
			continue;
		}

		if(c == '\n')
		{
			y += (i32)renderer->LineHeight;
			x = x0;
			continue;
		}

		chr = bsearch(&c, size->Characters, size->NumCharacters, sizeof(FontChar), _find_char);
		if(!chr)
		{
			continue;
		}

		if(c != ' ' && chr->Size[X] != 0 && chr->Size[Y] != 0)
		{
			src[0] = (i16)chr->Location[X];
			src[1] = (i16)chr->Location[Y];
			src[2] = (i16)chr->Size[X];
			src[3] = (i16)chr->Size[Y];

			dst[0] = (i16)(x + chr->Bearing[X]);
			dst[1] = (i16)(y + (size->Size - chr->Bearing[Y]));
			dst[2] = (i16)chr->Size[X];
			dst[3] = (i16)chr->Size[Y];

			renderer2d_sprite(dst, src, renderer->BoundTexture->Count - 1);
		}

		x += chr->Advance;
	}

	return x;
}

i32 font_width_string(const char *restrict s, u32 len)
{
	i32 c, width;
	u32 i;
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	FontSize *size = &renderer->BoundFC->Fonts[renderer->FontID].Sizes[renderer->SizeID];
	FontChar *chr;
	width = 0;
	for(i = 0; i < len; ++i)
	{
		s = utf8(s, &c);
		if(!c)
		{
			break;
		}

		chr = bsearch(&c, size->Characters, size->NumCharacters, sizeof(FontChar), _find_char);
		if(!chr)
		{
			continue;
		}

		width += chr->Advance;
	}

	return width;
}

i32 font_height_string(const char *restrict s, u32 len)
{
	i32 c, height, new;
	u32 i;
	Renderer2D *renderer = &GameEngine.Graphics.Renderer2D;
	FontSize *size = &renderer->BoundFC->Fonts[renderer->FontID].Sizes[renderer->SizeID];
	FontChar *chr;
	height = 0;
	for(i = 0; i < len; ++i)
	{
		s = utf8(s, &c);
		if(!c)
		{
			break;
		}

		chr = bsearch(&c, size->Characters, size->NumCharacters, sizeof(FontChar), _find_char);
		if(!chr)
		{
			continue;
		}

		new = chr->Bearing[Y] + chr->Size[Y];
		if(new > height)
		{
			height = new;
		}
	}

	return height;
}

