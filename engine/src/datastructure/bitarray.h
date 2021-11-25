#ifndef __BITARRAY_H__
#define __BITARRAY_H__

#include "types.h"

#define BITARRAY_TYPE         u64
#define BITARRAY_TYPE_BITS  64
#define BITARRAY_TYPE_SHIFT  6

typedef struct
{
	u32 Size;
	BITARRAY_TYPE *Bits;
} BitArray;

int bitarray_init(BitArray *bitarray, u32 initial_size);
int bitarray_resize(BitArray *bitarray, u32 new_size);
u32 bitarray_get(BitArray *bitarray, u32 bit);
int bitarray_set(BitArray *bitarray, u32 bit);
int bitarray_reset(BitArray *bitarray, u32 bit);
void bitarray_destroy(BitArray *bitarray);

#endif


