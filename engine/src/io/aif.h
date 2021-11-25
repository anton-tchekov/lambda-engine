#ifndef __AIF_H__
#define __AIF_H__

#include "types.h"

typedef struct AIF
{
	u8 *Data;
	u32 Count;
} AIF;

int aif_load_memory(u8 *buffer, AIF *aif);

#endif

