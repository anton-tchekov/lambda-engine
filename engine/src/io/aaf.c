#include "aaf.h"
#include <string.h>

#define AAF_VERSION_NO           0

static u8 _aaf_identifier[4] = { 'A', 'A', 'F', AAF_VERSION_NO };

int aaf_load_memory(u8 *file, AAF *aaf)
{
	if(memcmp(&file[0], _aaf_identifier, 4))
	{
		return 1;
	}

	aaf->Format = *(u32 *)&file[4];
	aaf->Length = *(u32 *)&file[8];
	aaf->Data = &file[12];
	return 0;
}

