#include "cubemap.h"
#include "debug.h"

Cubemap cubemap_load_acf_memory(u8 *buffer)
{
	ACF acf;
	if(acf_load_memory(buffer, &acf))
	{
		return 0;
	}

	return cubemap_load_acf(&acf);
}

Cubemap cubemap_load_acf(ACF *acf)
{
	u32 i, stride;
	Cubemap tid;
	GL_CHECK(glGenTextures(1, &tid));
	GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, tid));
	stride = 4 * (u32)acf->Size * (u32)acf->Size;
	for(i = 0; i < 6; ++i)
	{
		GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA8, acf->Size, acf->Size, 0, GL_BGRA, GL_UNSIGNED_BYTE, acf->Data + i * stride));
	}

	GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
	return tid;
}

