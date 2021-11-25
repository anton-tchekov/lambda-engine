#ifndef __BMP_H__
#define __BMP_H__

#include "types.h"

int bmp_write(const char *filename, const u8 *data, i32 width, i32 height);

#endif

