#ifndef __CRC_H__
#define __CRC_H__

#include "types.h"

u32 crc32_bytes(const void *s, u32 len);
u32 crc32_str(const char *s);

#endif

