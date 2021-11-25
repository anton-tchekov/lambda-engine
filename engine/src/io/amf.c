#include "amf.h"
#include "log.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define AMF_DEBUG */

#define STRIDE         32
#define AMF_VERSION_NO  0

static const u8 _amf_identifier[4] = { 'A', 'M', 'F', AMF_VERSION_NO };

int amf_load_memory(u8 *buffer, AMF *amf)
{
	u8 *p = buffer;
	u32 i, bytes;

	/* check identifier */
	if(memcmp(p, _amf_identifier, 4))
	{
		return 1;
	}

	p += 4;
	for(i = 0; i < NUM_TEXTURE_LAYERS; ++i)
	{
		amf->LayerNumTextures[i] = *(u32 *)p;
		p += 4;
	}

	for(i = 0; i < NUM_TEXTURE_LAYERS; ++i)
	{
		bytes = amf->LayerNumTextures[i] * 4 * (u32)_layer_sizes[i] * (u32)_layer_sizes[i];
		amf->Textures[i] = p;
		p += bytes;
	}

	amf->NumMeshes = *(u32 *)p;
	p += 4;
	amf->TotalMaterials = *(u32 *)p;
	p += 4;
	amf->TotalVertices = *(u32 *)p;
	p += 4;

	bytes = 2 * amf->NumMeshes * sizeof(u32);
	if(!(amf->MaterialOffsets = malloc(bytes)))
	{
		return 1;
	}

	amf->MaterialCounts = amf->MaterialOffsets + amf->NumMeshes;
	memcpy(amf->MaterialOffsets, p, bytes);
	p += bytes;

	bytes = 14 * amf->TotalMaterials * sizeof(u32);
	if(!(amf->Materials = malloc(bytes)))
	{
		return 1;
	}

	memcpy(amf->Materials, p, bytes);
	p += bytes;

	amf->Vertices = p;
	p += amf->TotalVertices * STRIDE;

	bytes = amf->NumMeshes * sizeof(AABB);
	if(!(amf->AABBs = malloc(bytes)))
	{
		return 1;
	}

	for(i = 0; i < amf->NumMeshes; ++i)
	{
		amf->AABBs[i].VelocityY = 0.0f;
		memcpy(amf->AABBs[i].Min, p, sizeof(vec3));
		p += sizeof(vec3);
		memcpy(amf->AABBs[i].Max, p, sizeof(vec3));
		p += sizeof(vec3);

#ifdef AMF_DEBUG
		printf(
			"i: %d\n"
			"vY: %f\n"
			"min: { %f %f %f }\n"
			"max: { %f %f %f }\n\n",
			i, amf->AABBs[i].VelocityY,
			amf->AABBs[i].Min[X], amf->AABBs[i].Min[Y], amf->AABBs[i].Min[Z],
			amf->AABBs[i].Max[X], amf->AABBs[i].Max[Y], amf->AABBs[i].Max[Z]);
#endif
	}

	return 0;
}

