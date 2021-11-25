#ifndef __AAF_H__
#define __AAF_H__

#include "types.h"

typedef struct AAF
{
	u32 Format, Length;
	u8 *Data;
} AAF;

int aaf_load_memory(u8 *file, AAF *aaf);

#endif

