#include "bitarray.h"
#include <stdlib.h>
#include <string.h>

static u32 _elements(u32 bit);
static u32 _element(u32 bit);
static u32 _bit(u32 bit);

int bitarray_init(BitArray *bitarray, u32 initial_size)
{
	u32 elements;
	bitarray->Size = initial_size;
	elements = _elements(initial_size);
	if(!(bitarray->Bits = calloc(elements, sizeof(BITARRAY_TYPE))))
	{
		return -1;
	}

	return 0;
}

int bitarray_resize(BitArray *bitarray, u32 new_size)
{
	u32 byte;
	BITARRAY_TYPE *new_bits;
	byte = _elements(new_size);
	if(!(new_bits = realloc(bitarray->Bits, byte * sizeof(BITARRAY_TYPE))))
	{
		return -1;
	}

	memset(bitarray->Bits + byte, 0, new_size - bitarray->Size);
	bitarray->Size = new_size;
	return 0;
}

u32 bitarray_get(BitArray *bitarray, u32 bit)
{
	return bit >= bitarray->Size ? 0 : bitarray->Bits[_element(bit)] & (1 << _bit(bit));
}

int bitarray_set(BitArray *bitarray, u32 bit)
{
	if(bit >= bitarray->Size)
	{
		return -1;
	}

	bitarray->Bits[_element(bit)] |= (1 << _bit(bit));
	return 0;
}

int bitarray_reset(BitArray *bitarray, u32 bit)
{
	if(bit >= bitarray->Size)
	{
		return -1;
	}

	bitarray->Bits[_element(bit)] &= ~(1 << _bit(bit));
	return 0;
}

void bitarray_destroy(BitArray *bitarray)
{
	free(bitarray->Bits);
}

static u32 _elements(u32 bit)
{
	return ((bit + BITARRAY_TYPE_BITS - 1) & ~(BITARRAY_TYPE_BITS - 1)) >> BITARRAY_TYPE_SHIFT;
}

static u32 _element(u32 bit)
{
	return bit >> BITARRAY_TYPE_SHIFT;
}

static u32 _bit(u32 bit)
{
	return bit & (BITARRAY_TYPE_BITS - 1);
}

