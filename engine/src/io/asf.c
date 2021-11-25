#include "asf.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASF_VERSION_NO           0

static const u8 _asf_identifier[4] = { 'A', 'S', 'F', ASF_VERSION_NO };

static int _asf_alloc(ShadowCollection *asf)
{
	u32 i, max = 0;
	for(i = 0; i < asf->NumShadows; ++i)
	{
		if(asf->VertexCounts[i] > max)
		{
			max = asf->VertexCounts[i];
		}
	}

	return !(asf->Transformed = malloc(max * sizeof(vec3)));
}

int asf_load_memory(u8 *buffer, ShadowCollection *asf)
{
	u32 i;
	if(memcmp(buffer, _asf_identifier, 4))
	{
		return 1;
	}

	asf->Base = buffer;
	i = 4;
	asf->NumShadows = *(u32 *)(buffer + i);
	i += 4;
	asf->TotalVertices = *(u32 *)(buffer + i);
	i += 4;
	asf->TotalIndices = *(u32 *)(buffer + i);
	i += 4;
	asf->TotalEdges = *(u32 *)(buffer + i);
	i += 4;
	asf->VertexOffsets = (u32 *)(buffer + i);
	asf->VertexCounts = asf->VertexOffsets + asf->NumShadows;
	asf->IndexOffsets = asf->VertexCounts + asf->NumShadows;
	asf->IndexCounts = asf->IndexOffsets + asf->NumShadows;
	asf->EdgeOffsets = asf->IndexCounts + asf->NumShadows;
	asf->EdgeCounts = asf->EdgeOffsets + asf->NumShadows;
	i += 4 * 6 * asf->NumShadows;
	asf->Vertices = (vec3 *)(buffer + i);
	i += 4 * 3 * asf->TotalVertices;
	asf->Indices = (u32 *)(buffer + i);
	i += 4 * asf->TotalIndices;
	asf->Edges = (ShadowEdge *)(buffer + i);
	if(_asf_alloc(asf))
	{
		return 1;
	}

	return 0;
}

