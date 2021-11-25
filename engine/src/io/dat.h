#ifndef __COMPRESSION_H__
#define __COMPRESSION_H__

#include "types.h"
#include "audio.h"
#include "texture.h"
#include "shadow.h"
#include "mesh.h"
#include "opengl.h"
#include "font.h"
#include "asf.h"
#include "cubemap.h"

typedef struct DAT
{
	u32 Size;
	u8 *Data;
} DAT;

int dat_load_file(const char *filename, DAT *dat);

/* Compressed loader functions */
int audio_load(const char *filename, AudioBuffer *audio);

int meshes_load(const char *filename, MeshCollection *meshes);
void meshes_destroy(MeshCollection *meshes);

int shadows_load(const char *filename, ShadowCollection *shadow);
void shadows_destroy(ShadowCollection *shadow);

int texture_load(const char *filename, Texture *texture, u32 flags);
void texture_destroy(Texture *texture);

int cubemap_load(const char *filename, Cubemap *cubemap);
void cubemap_destroy(Cubemap *cubemap);

int fonts_load(const char *filename, FontCollection *fonts);
void fonts_destroy(FontCollection *fonts);

#endif

