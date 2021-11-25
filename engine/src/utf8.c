#include "utf8.h"

const char *utf8(const char *restrict s, i32 *restrict out)
{
	if((s[0] & 0xf8) == 0xf0)
	{
		*out = ((s[0] & 0x07) << 18) | ((s[1] & 0x3f) << 12) | ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
		s += 4;
	}
	else if((s[0] & 0xf0) == 0xe0)
	{
		*out = ((s[0] & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f);
		s += 3;
	}
	else if((s[0] & 0xe0) == 0xc0)
	{
		*out = ((s[0] & 0x1f) << 6) | (s[1] & 0x3f);
		s += 2;
	}
	else
	{
		*out = s[0];
		s += 1;
	}

	return s;
}

