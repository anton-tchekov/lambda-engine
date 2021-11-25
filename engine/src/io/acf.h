#ifndef __ACF_H__
#define __ACF_H__

#include "types.h"

typedef struct ACF
{
	u8 *Data;
	i32 Size;
} ACF;

int acf_load_memory(u8 *file, ACF *acf);

#endif

