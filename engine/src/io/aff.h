#ifndef __AFF_H__
#define __AFF_H__

#include "types.h"

typedef struct FONT_CHAR
{
	i32 Character, Advance, Location[2], Size[2], Bearing[2];
} FontChar;

typedef struct FONT_SIZE
{
	FontChar *Characters;
	i32 Size;
	u32 NumCharacters;
} FontSize;

typedef struct FONT
{
	FontSize *Sizes;
	u32 NumSizes;
} Font;

typedef struct AFF
{
	u32 NumFonts, TextureSize;
	Font *Fonts;
} AFF;

int aff_load_memory(u8 *file, AFF *aff);

#endif

