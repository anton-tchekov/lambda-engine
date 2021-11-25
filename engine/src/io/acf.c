#include "acf.h"
#include <string.h>

#define ACF_VERSION_NO           0

static const u8 _acf_identifier[4] = { 'A', 'C', 'F', ACF_VERSION_NO };

int acf_load_memory(u8 *file, ACF *acf)
{
	if(memcmp(&file[0], _acf_identifier, 4))
	{
		return 1;
	}

	acf->Size = 1 << (*(u32 *)&file[4]);
	acf->Data = &file[8];
	return 0;
}

