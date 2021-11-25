#include "texture.h"
#include "log.h"
#include "debug.h"
#include "internal.h"
#include <stdlib.h>

int texture_load_aif_memory(u8 *buffer, Texture *texture, u32 flags)
{
	AIF aif;
	if(aif_load_memory(buffer, &aif))
	{
		return 1;
	}

	texture_load_aif(&aif, texture, flags);
	return 0;
}

void texture_load_aif(const AIF *aif, Texture *texture, u32 flags)
{
	GLuint tid;
	GL_CHECK(glGenTextures(1, &tid));
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_BIND));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, tid));
	GL_CHECK(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 1024, 1024,
			(i32)aif->Count, 0, GL_BGRA, GL_UNSIGNED_BYTE, aif->Data));

	if(flags & TEXTURE_CLAMP)
	{
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	}
	else
	{
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	if(flags & TEXTURE_NEAREST)
	{
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	else
	{
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}

	texture->TextureID = tid;
	texture->Size = (GLsizei)1024;
	texture->Count = (GLsizei)aif->Count;
}

