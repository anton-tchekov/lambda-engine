#include "dat.h"
#include "zlib.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DAT_VERSION_NO 0

static u8 _dat_identifier[4] = { 'D', 'A', 'T', DAT_VERSION_NO };

int dat_load_file(const char *filename, DAT *dat)
{
	int rv;
	unsigned int csize;
	unsigned long usize;
	u8 header[12], *cdata, *udata;
	FILE *fp;
	if(!(fp = fopen(filename, "rb")))
	{
		return 1;
	}

	if(fread(header, sizeof(u8), 12, fp) != 12)
	{
		goto fail_file;
	}

	/* check identifier */
	if(memcmp(&header[0], _dat_identifier, 4))
	{
		goto fail_file;
	}

	usize = *(u32 *)&header[4];
	csize = *(u32 *)&header[8];
	if(!(cdata = malloc(csize)))
	{
		goto fail_file;
	}

	if(!(udata = malloc(usize)))
	{
		goto fail_alloc0;
	}

	if(fread(cdata, 1, csize, fp) != csize)
	{
		goto fail_alloc1;
	}

	if((rv = uncompress(udata, &usize, cdata, csize)) != Z_OK)
	{
		goto fail_alloc1;
	}

	dat->Size = (u32)usize;
	dat->Data = udata;
	free(cdata);
	fclose(fp);
	return 0;

fail_alloc1:
	free(udata);

fail_alloc0:
	free(cdata);

fail_file:
	fclose(fp);
	return 1;
}

int audio_load(const char *filename, AudioBuffer *audio)
{
	DAT data;
	if(!audio_enabled())
	{
		return 0;
	}

	log_debug("Loading audio \"%s\"", filename);
	if(dat_load_file(filename, &data))
	{
		return 1;
	}

	if(!(*audio = audio_buffer_aaf_memory(data.Data)))
	{
		free(data.Data);
		return 1;
	}

	free(data.Data);
	return 0;
}

int meshes_load(const char *filename, MeshCollection *meshes)
{
	DAT data;
	log_debug("Loading meshes \"%s\"", filename);
	if(dat_load_file(filename, &data))
	{
		return 1;
	}

	if(mesh_collection_load_amf_memory(data.Data, meshes))
	{
		free(data.Data);
		return 1;
	}

	free(data.Data);
	return 0;
}

int shadows_load(const char *filename, ShadowCollection *shadow)
{
	DAT data;
	log_debug("Loading shadows \"%s\"", filename);
	if(dat_load_file(filename, &data))
	{
		return 1;
	}

	if(asf_load_memory(data.Data, shadow))
	{
		free(data.Data);
		return 1;
	}

	return 0;
}

void shadows_destroy(ShadowCollection *shadow)
{
	if(shadow->Base)
	{
		free(shadow->Base);
	}
	else
	{
		free(shadow->Edges);
		free(shadow->Indices);
		free(shadow->Vertices);
		free(shadow->VertexOffsets);
	}
}

int texture_load(const char *filename, Texture *texture, u32 flags)
{
	DAT data;
	log_debug("Loading texture \"%s\"", filename);
	if(dat_load_file(filename, &data))
	{
		return 1;
	}

	if(texture_load_aif_memory(data.Data, texture, flags))
	{
		free(data.Data);
		return 1;
	}

	free(data.Data);
	return 0;
}

void texture_destroy(Texture *texture)
{
	GL_CHECK(glDeleteTextures(1, &texture->TextureID));
}

int cubemap_load(const char *filename, Cubemap *cubemap)
{
	DAT data;
	log_debug("Loading cubemap \"%s\"", filename);
	if(dat_load_file(filename, &data))
	{
		return 1;
	}

	if(!(*cubemap = cubemap_load_acf_memory(data.Data)))
	{
		free(data.Data);
		return 1;
	}

	free(data.Data);
	return 0;
}

void cubemap_destroy(Cubemap *cubemap)
{
	GL_CHECK(glDeleteTextures(1, cubemap));
}

int fonts_load(const char *filename, FontCollection *fonts)
{
	DAT data;
	log_debug("Loading font \"%s\"", filename);
	if(dat_load_file(filename, &data))
	{
		return 1;
	}

	if(font_collection_load_aff_memory(data.Data, fonts))
	{
		free(data.Data);
		return 1;
	}

	free(data.Data);
	return 0;
}

