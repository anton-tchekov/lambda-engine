#ifndef __COLOR_H__
#define __COLOR_H__

#include "types.h"
#include "vector.h"

typedef u8 ColorRGBA8[4];

struct COLORS
{
	vec4 RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK, TRANSPRNT;
};

struct COLORS8
{
	ColorRGBA8 RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK, TRANSPRNT;
};

extern const struct COLORS COLOR;
extern const struct COLORS8 COLOR8;

#endif

