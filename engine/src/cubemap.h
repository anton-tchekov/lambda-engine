#ifndef __CUBEMAP_H__
#define __CUBEMAP_H__

#include "types.h"
#include "opengl.h"
#include "acf.h"

typedef GLuint Cubemap;

Cubemap cubemap_load_acf_memory(u8 *buffer);
Cubemap cubemap_load_acf(ACF *acf);

#endif

