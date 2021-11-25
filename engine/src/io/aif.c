#include "aif.h"
#include <string.h>

#define AIF_VERSION_NO           0

static const u8 _aif_identifier[4] = { 'A', 'I', 'F', AIF_VERSION_NO };

int aif_load_memory(u8 *buffer, AIF *aif)
{
	if(memcmp(&buffer[0], _aif_identifier, 4))
	{
		return 1;
	}

	aif->Count = *(u32 *)&buffer[4];
	aif->Data = &buffer[8];
	return 0;
}

