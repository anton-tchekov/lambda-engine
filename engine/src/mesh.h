#ifndef __MESH_H__
#define __MESH_H__

#include "types.h"
#include "aabb.h"
#include "opengl.h"
#include "amf.h"

#define NUM_TEXTURE_LAYERS          6

typedef struct MESH_COLLECTION
{
	u32 NumMeshes, TotalVertices, TotalMaterials, *MaterialOffsets, *MaterialCounts;
	AABB *AABBs;
	Material *Materials;
	GLuint VBO, VAO, Textures[NUM_TEXTURE_LAYERS];
} MeshCollection;

int mesh_collection_load_amf_memory(u8 *buffer, MeshCollection *mc);
int mesh_collection_load_amf(const AMF *amf, MeshCollection *mc);

#endif

