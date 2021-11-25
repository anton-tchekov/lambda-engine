#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <math.h>

#include "vector.h"
#include "types.h"

struct Vertex
{
	float x, y, z;
	int v;
};

struct Edge
{
	int v0, v1, f0, f1;
};

static float distance2(float x0, float y0, float z0, float x1, float y1, float z1);
static int find_edge(std::vector<Edge> &edges, int v0, int v1);
static int _gen_shadow_prot(const aiMesh *mesh,
		std::vector<Vertex> &vertices,
		std::vector<u32> &indices,
		std::vector<Edge> &edges);

static int asf_write(char *filename, u32 num_shadows,
		std::vector<Vertex> vertices[],
		std::vector<u32> indices[],
		std::vector<Edge> edges[]);

static int _write_vertex(FILE *fp, Vertex *vertex);

int main(int argc, char *argv[])
{
	int i, n;
	Assimp::Importer importer;
	const aiScene *scene;

	assert(sizeof(float) == 4);
	assert(sizeof(int) == 4);
	assert(sizeof(Vertex) == 16);
	assert(sizeof(Edge) == 16);

	if(argc < 3)
	{
		printf("Usage: ./asf_create out in-0 ...\n");
		return 1;
	}

	n = argc - 2;
	std::vector<Vertex> vertices[n];
	std::vector<u32> indices[n];
	std::vector<Edge> edges[n];

	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS | aiComponent_TANGENTS_AND_BITANGENTS |
			aiComponent_COLORS | aiComponent_TEXCOORDS | aiComponent_BONEWEIGHTS | aiComponent_ANIMATIONS |
			aiComponent_TEXTURES | aiComponent_LIGHTS | aiComponent_CAMERAS | aiComponent_MATERIALS);

	for(i = 0; i < n; ++i)
	{
		printf("Generating Shadow Geometry for Mesh #%d\n", i);
		scene = importer.ReadFile(argv[i + 2], aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_RemoveComponent);
		if(_gen_shadow_prot(scene->mMeshes[0], vertices[i], indices[i], edges[i]))
		{
			fprintf(stderr, "Error: Invalid (possibly unclosed) geometry\n");
			return 1;
		}
	}

	asf_write(argv[1], n, vertices, indices, edges);
	return 0;
}

static int asf_write(char *filename, u32 num_shadows,
		std::vector<Vertex> vertices[],
		std::vector<u32> indices[],
		std::vector<Edge> edges[])
{
	static u8 _asf_header[] = { 'A', 'S', 'F', 0 };
	FILE *fp;
	u32 i, j, size, offset, total_vertices, total_indices, total_edges;

	fprintf(stderr, "Opening output file \"%s\" for writing\n", filename);
	if(!(fp = fopen(filename, "wb")))
	{
		fprintf(stderr, "Failed to open output file \"%s\"\n", filename);
		return 1;
	}

	fprintf(stderr, "Writing ASF header\n");
	if(fwrite(_asf_header, 1, 4, fp) != 4)
	{
		fprintf(stderr, "Failed to write ASF identifier\n");
		goto fail_file;
	}

	if(fwrite(&num_shadows, 4, 1, fp) != 1)
	{
		fprintf(stderr, "Failed to write number of shadow volume prototypes\n");
		goto fail_file;
	}

	total_vertices = 0;
	total_indices = 0;
	total_edges = 0;
	for(i = 0; i < num_shadows; ++i)
	{
		total_vertices += vertices[i].size();
		total_indices += indices[i].size();
		total_edges += edges[i].size();
	}

	fprintf(stderr, "Number of shadow prototypes: %d\n", num_shadows);

	fprintf(stderr, "Total number of vertices: %d\n", total_vertices);
	fprintf(stderr, "Total number of indices: %d\n", total_indices);
	fprintf(stderr, "Total number of edges: %d\n", total_edges);

	if(fwrite(&total_vertices, 4, 1, fp) != 1)
	{
		fprintf(stderr, "Failed to write number total number of vertices\n");
		goto fail_file;
	}

	if(fwrite(&total_indices, 4, 1, fp) != 1)
	{
		fprintf(stderr, "Failed to write number total number of indices\n");
		goto fail_file;
	}

	if(fwrite(&total_edges, 4, 1, fp) != 1)
	{
		fprintf(stderr, "Failed to write number total number of edges\n");
		goto fail_file;
	}

	/* OFFSET / SIZE TABLE */
	fprintf(stderr, "Writing shadow vertex offset/size table\n");
	for(offset = 0, i = 0; i < num_shadows; ++i)
	{
		size = vertices[i].size();
		if(fwrite(&offset, 4, 1, fp) != 1)
		{
			fprintf(stderr, "Failed to write vertex offset #%d\n", i);
			goto fail_file;
		}

		printf("Offset: %d\n", offset);
		offset += size;
	}

	for(i = 0; i < num_shadows; ++i)
	{
		size = vertices[i].size();
		if(fwrite(&size, 4, 1, fp) != 1)
		{
			fprintf(stderr, "Failed to write vertex size #%d\n", i);
			goto fail_file;
		}

		printf("Size: %d\n", size);
	}

	fprintf(stderr, "Writing shadow index offset/size table\n");
	for(offset = 0, i = 0; i < num_shadows; ++i)
	{
		size = indices[i].size();
		if(fwrite(&offset, 4, 1, fp) != 1)
		{
			fprintf(stderr, "Failed to write index offset #%d\n", i);
			goto fail_file;
		}

		printf("Offset: %d\n", offset);
		offset += size;
	}

	for(i = 0; i < num_shadows; ++i)
	{
		size = indices[i].size();
		if(fwrite(&size, 4, 1, fp) != 1)
		{
			fprintf(stderr, "Failed to write index size #%d\n", i);
			goto fail_file;
		}

		printf("Size: %d\n", size);
	}

	fprintf(stderr, "Writing shadow edge offset/size table\n");
	for(offset = 0, i = 0; i < num_shadows; ++i)
	{
		size = edges[i].size();
		if(fwrite(&offset, 4, 1, fp) != 1)
		{
			fprintf(stderr, "Failed to write edge offset #%d\n", i);
			goto fail_file;
		}

		printf("Offset: %d\n", offset);
		offset += size;
	}

	for(i = 0; i < num_shadows; ++i)
	{
		size = edges[i].size();
		if(fwrite(&size, 4, 1, fp) != 1)
		{
			fprintf(stderr, "Failed to write edge size #%d\n", i);
			goto fail_file;
		}

		printf("Size: %d\n", size);
	}

	/* BODY DATA - VERTICES, INDICES, EDGES */
	fprintf(stderr, "Writing shadow prototype body\n");
	fprintf(stderr, "Writing shadow vertex data\n");
	for(i = 0; i < num_shadows; ++i)
	{
		for(j = 0; j < vertices[i].size(); ++j)
		{
			if(_write_vertex(fp, &vertices[i][j]))
			{
				fprintf(stderr, "Failed to write vertex data #%d\n", i);
				goto fail_file;
			}
		}
	}

	fprintf(stderr, "Writing shadow index data\n");
	for(i = 0; i < num_shadows; ++i)
	{
		if(fwrite(&indices[i][0], sizeof(u32), indices[i].size(), fp) != indices[i].size())
		{
			fprintf(stderr, "Failed to write index data #%d\n", i);
			goto fail_file;
		}
	}

	fprintf(stderr, "Writing shadow edge data\n");
	for(i = 0; i < num_shadows; ++i)
	{
		if(fwrite(&edges[i][0], sizeof(Edge), edges[i].size(), fp) != edges[i].size())
		{
			fprintf(stderr, "Failed to write edge data #%d\n", i);
			goto fail_file;
		}
	}

	return 0;

fail_file:
	fclose(fp);
	return 1;
}

static int _write_vertex(FILE *fp, Vertex *vertex)
{
	if(fwrite(&vertex->x, sizeof(float), 1, fp) != 1)
	{
		return 1;
	}

	if(fwrite(&vertex->y, sizeof(float), 1, fp) != 1)
	{
		return 1;
	}

	if(fwrite(&vertex->z, sizeof(float), 1, fp) != 1)
	{
		return 1;
	}

	return 0;
}

static float distance2(float x0, float y0, float z0, float x1, float y1, float z1)
{
	return (x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1) + (z0 - z1) * (z0 - z1);
}

static int find_edge(std::vector<Edge> &edges, int v0, int v1)
{
	u32 i;
	for(i = 0; i < edges.size(); ++i)
	{
		if(edges[i].v0 == v0 && edges[i].v1 == v1)
		{
			return i;
		}
	}

	return -1;
}

static int _gen_shadow_prot(const aiMesh *mesh,
		std::vector<Vertex> &vertices,
		std::vector<u32> &indices,
		std::vector<Edge> &edges)
{
	u32 i, j;
	int e, f, v0, v1, v2;
	Vertex vertex;
	Edge edge;

	for(i = 0; i < mesh->mNumVertices; ++i)
	{
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		vertex.v = 0;
		vertices.push_back(vertex);
	}

	for(i = 0; i < mesh->mNumFaces; ++i)
	{
		for(j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	/* Map similar vertices */
	for(i = 0; i < vertices.size(); ++i)
	{
		for(j = 0; j <= i; ++j)
		{
			if(distance2(vertices[j].x, vertices[j].y, vertices[j].z,
				vertices[i].x, vertices[i].y, vertices[i].z) < EPSILON)
			{
				vertices[i].v = j;
				break;
			}
		}
	}

	/* Create edge list */
	for(i = 0, f = 0; i < indices.size(); i += 3, ++f)
	{
		v0 = vertices[indices[i]].v;
		v1 = vertices[indices[i + 1]].v;
		v2 = vertices[indices[i + 2]].v;
		if((e = find_edge(edges, v0, v1)) > -1)
		{
			edges[e].f1 = f;
		}
		else
		{
			edge.v0 = v1;
			edge.v1 = v0;
			edge.f0 = f;
			edge.f1 = -1;
			edges.push_back(edge);
		}

		if((e = find_edge(edges, v1, v2)) > -1)
		{
			edges[e].f1 = f;
		}
		else
		{
			edge.v0 = v2;
			edge.v1 = v1;
			edge.f0 = f;
			edge.f1 = -1;
			edges.push_back(edge);
		}

		if((e = find_edge(edges, v2, v0)) > -1)
		{
			edges[e].f1 = f;
		}
		else
		{
			edge.v0 = v0;
			edge.v1 = v2;
			edge.f0 = f;
			edge.f1 = -1;
			edges.push_back(edge);
		}
	}

	for(i = 0; i < edges.size(); ++i)
	{
		if(edges[i].f0 < 0 || edges[i].f1 < 0)
		{
			return 1;
		}
	}

	return 0;
}

