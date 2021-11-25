#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "types.h"
#include "opengl.h"
#include "aif.h"

typedef struct TEXTURE
{
	GLsizei Size, Count;
	GLuint TextureID;
} Texture;

#define TEXTURE_LINEAR  (0 << 0)
#define TEXTURE_NEAREST (1 << 0)

#define TEXTURE_REPEAT  (0 << 1)
#define TEXTURE_CLAMP   (1 << 1)

int texture_load_aif_memory(u8 *buffer, Texture *texture, u32 flags);
void texture_load_aif(const AIF *aif, Texture *texture, u32 flags);

#endif

