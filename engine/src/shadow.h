#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "types.h"
#include "vector.h"

#define MAX_SHADOW_INDICES    (1024 * 1024)
#define MAX_SHADOW_VERTICES   (1024 * 1024)
#define SHADOW_INDICES_BYTES  (MAX_SHADOW_INDICES * sizeof(u32))
#define SHADOW_VERTICES_BYTES (MAX_SHADOW_VERTICES * sizeof(vec3))

typedef struct SHADOW_EDGE
{
	u32 V0, V1, F0, F1;
} ShadowEdge;

typedef struct SHADOW_COLLECTION
{
	u8 *Base;
	u32 NumShadows, TotalVertices, TotalIndices, TotalEdges;
	u32 *VertexOffsets, *VertexCounts, *IndexOffsets, *IndexCounts, *EdgeOffsets, *EdgeCounts;
	vec3 *Vertices;
	vec3 *Transformed;
	u32 *Indices;
	ShadowEdge *Edges;
} ShadowCollection;

void shadow_volume_generate(vec3 light_pos, mat4 model, u32 id);

#endif

