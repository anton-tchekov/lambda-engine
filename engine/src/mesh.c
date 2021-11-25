#include "mesh.h"
#include "debug.h"
#include "internal.h"
#include <stdio.h>

#define STRIDE 32

/* #define MESH_DEBUG */

const GLsizei _layer_sizes[NUM_TEXTURE_LAYERS] = { 32, 64, 128, 256, 512, 1024 };

int mesh_collection_load_amf_memory(u8 *buffer, MeshCollection *mc)
{
	AMF amf;
	if(amf_load_memory(buffer, &amf))
	{
		return 1;
	}

	if(mesh_collection_load_amf(&amf, mc))
	{
		return 1;
	}

	return 0;
}

int mesh_collection_load_amf(const AMF *amf, MeshCollection *mc)
{
	u32 i;
	for(i = 0; i < NUM_TEXTURE_LAYERS; ++i)
	{
		if(amf->LayerNumTextures[i])
		{
			GL_CHECK(glGenTextures(1, &mc->Textures[i]));
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_BIND));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, mc->Textures[i]));
			GL_CHECK(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, _layer_sizes[i], _layer_sizes[i],
					(i32)amf->LayerNumTextures[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, amf->Textures[i]));

			GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT));
		}
	}

	mc->NumMeshes = amf->NumMeshes;
	mc->TotalVertices = amf->TotalVertices;
	mc->TotalMaterials = amf->TotalMaterials;

	mc->MaterialOffsets = amf->MaterialOffsets;
	mc->MaterialCounts = amf->MaterialCounts;

	mc->Materials = amf->Materials;

	mc->AABBs = amf->AABBs;

#ifdef MESH_DEBUG
	fprintf(stderr, "num_meshes = %d\n", mc->NumMeshes);
	fprintf(stderr, "total_vertices = %d\n", mc->TotalVertices);
	fprintf(stderr, "total_materials = %d\n", mc->TotalMaterials);

	for(i = 0; i < mc->NumMeshes; ++i)
	{
		fprintf(stderr, "offsets = %d\n", mc->MaterialOffsets[i]);
		fprintf(stderr, "count = %d\n", mc->MaterialCounts[i]);
	}

	fprintf(stderr, "ID   | Offset   | Size     | DColor  | SColor  | EColor  | DSI | SSI | ESI | NSI | DLY | SLY | ELY | NLY | SHINY    |\n");
	for(i = 0; i < mc->TotalMaterials; ++i)
	{
		Material *m = &mc->Materials[i];
		fprintf(stderr, "%4d | %8d | %8d | #%02x%02x%02x | #%02x%02x%02x | #%02x%02x%02x | %3d | %3d | %3d | %3d | %3d | %3d | %3d | %3d | %8.4f |\n",
				i, m->VertexOffset, m->VertexCount,
				m->ColorDiffuse[0], m->ColorDiffuse[1], m->ColorDiffuse[2],
				m->ColorSpecular[0], m->ColorSpecular[1], m->ColorSpecular[2],
				m->ColorEmissive[0], m->ColorEmissive[1], m->ColorEmissive[2],
				m->SizeDiffuse, m->SizeSpecular, m->SizeEmissive, m->SizeNormal,
				m->LayerDiffuse, m->LayerSpecular, m->LayerEmissive, m->LayerNormal,
				m->Shininess);
	}
#endif

	GL_CHECK(glGenVertexArrays(1, &mc->VAO));
	GL_CHECK(glGenBuffers(1, &mc->VBO));
	GL_CHECK(glBindVertexArray(mc->VAO));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mc->VBO));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, STRIDE * (GLsizeiptr)amf->TotalVertices, amf->Vertices, GL_STATIC_DRAW));

	/* Vertex Specification */
	/* Position */
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, (void *)0));

	/* Normal */
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 4, GL_INT_2_10_10_10_REV, GL_TRUE, STRIDE, (void *)12));

	/* Texture Coordinates */
	GL_CHECK(glEnableVertexAttribArray(2));
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE, (void *)16));

	/* Tangent */
	GL_CHECK(glEnableVertexAttribArray(3));
	GL_CHECK(glVertexAttribPointer(3, 4, GL_INT_2_10_10_10_REV, GL_TRUE, STRIDE, (void *)24));

	/* Bitangent */
	GL_CHECK(glEnableVertexAttribArray(4));
	GL_CHECK(glVertexAttribPointer(4, 4, GL_INT_2_10_10_10_REV, GL_TRUE, STRIDE, (void *)28));

	return 0;
}

