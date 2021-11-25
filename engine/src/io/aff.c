#include "aff.h"
#include <stdlib.h>
#include <string.h>

#define AFF_VERSION_NO           0

static u8 _aff_identifier[4] = { 'A', 'F', 'F', AFF_VERSION_NO };

int aff_load_memory(u8 *file, AFF *aff)
{
	u32 i, j, k, num_font_allocs, num_size_allocs, offset;
	if(memcmp(file, _aff_identifier, 4))
	{
		return 1;
	}

	offset = 4;
	aff->TextureSize = 1U << (*(u32 *)(file + offset));
	offset += 4;
	aff->NumFonts = *(u32 *)(file + offset);
	offset += 4;
	if(!(aff->Fonts = malloc(aff->NumFonts * sizeof(Font))))
	{
		return 1;
	}

	for(num_font_allocs = 0; num_font_allocs < aff->NumFonts; ++num_font_allocs)
	{
		aff->Fonts[num_font_allocs].NumSizes = *(u32 *)(file + offset);
		offset += 4;
		if(!(aff->Fonts[num_font_allocs].Sizes =
				malloc(aff->Fonts[num_font_allocs].NumSizes * sizeof(FontSize))))
		{
			goto fail_alloc0;
		}
	}

	for(i = 0; i < aff->NumFonts; ++i)
	{
		for(num_size_allocs = 0; num_size_allocs < aff->Fonts[i].NumSizes; ++num_size_allocs)
		{
			aff->Fonts[i].Sizes[num_size_allocs].Size = *(i32 *)(file + offset);
			offset += 4;
			aff->Fonts[i].Sizes[num_size_allocs].NumCharacters = *(u32 *)(file + offset);
			offset += 4;

			if(!(aff->Fonts[i].Sizes[num_size_allocs].Characters =
					malloc(aff->Fonts[i].Sizes[num_size_allocs].NumCharacters * sizeof(FontChar))))
			{
				goto fail_alloc1;
			}
		}
	}

	for(i = 0; i < aff->NumFonts; ++i)
	{
		for(j = 0; j < aff->Fonts[i].NumSizes; ++j)
		{
			for(k = 0; k < aff->Fonts[i].Sizes[j].NumCharacters; ++k)
			{
				memcpy(&aff->Fonts[i].Sizes[j].Characters[k], file + offset, 32);
				offset += 32;
			}
		}
	}

	return 0;

fail_alloc1:
	for(i = 0; i < num_font_allocs; ++i)
	{
		for(j = 0; j < num_size_allocs; ++j)
		{
			free(aff->Fonts[i].Sizes[j].Characters);
		}
	}

fail_alloc0:
	for(i = 0; i < num_font_allocs; ++i)
	{
		free(aff->Fonts[i].Sizes);
	}

	free(aff->Fonts);
	return 1;
}

