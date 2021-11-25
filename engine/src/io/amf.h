#ifndef __AMF_H__
#define __AMF_H__

#include "types.h"
#include "aabb.h"
#include "defines.h"

typedef struct MATERIAL
{
	u32 VertexOffset, VertexCount;
	u8 ColorDiffuse[4], ColorSpecular[4], ColorEmissive[4];
	u32 SizeDiffuse, SizeSpecular, SizeEmissive, SizeNormal;
	u32 LayerDiffuse, LayerSpecular, LayerEmissive, LayerNormal;
	float Shininess;
} Material;

typedef struct AMF
{
	u32 NumMeshes, TotalVertices, TotalMaterials, LayerNumTextures[NUM_TEXTURE_LAYERS];
	u32 *MaterialOffsets, *MaterialCounts;
	u8 *Textures[NUM_TEXTURE_LAYERS], *Vertices;
	Material *Materials;
	AABB *AABBs;
} AMF;

int amf_load_memory(u8 *buffer, AMF *amf);

#endif

