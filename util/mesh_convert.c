#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include "readfile.h"
#include "vector.h"
#include "types.h"
#include "bmp.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "lib/objloader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/image.h"

#define TEXTURE_NUM_SIZES         6
#define MIN_LAYER_SIZE           32
#define MAX_LAYER_SIZE         1024
#define TEXTURES_PER_MATERIAL     4
#define TEXTURE_DIFFUSE           0
#define TEXTURE_SPECULAR          1
#define TEXTURE_EMISSIVE          2
#define TEXTURE_NORMAL            3
#define STRIDE                   32
#define SIZE_ID(X)                 (ilog2(X) - 5)

typedef struct TEXTURE
{
	struct TEXTURE *Dupe;
	int Layer, Size;
	u8 *Data;
} Texture;

typedef struct MESH3D
{
	int num_shapes, num_materials;
	tinyobj_attrib_t attrs;
	tinyobj_shape_t *shapes;
	tinyobj_material_t *materials;
	Texture *textures;
} Mesh3D;

typedef struct AABB
{
	vec3 Min, Max;
} AABB;

/* 14 Longs */
typedef struct MATERIAL
{
	u32 VertexOffset, VertexCount;
	u8 ColorDiffuse[4], ColorSpecular[4], ColorEmissive[4];
	u32 DiffuseSizeID, SpecularSizeID, EmissiveSizeID, NormalSizeID;
	u32 DiffuseLayer, SpecularLayer, EmissiveLayer, NormalLayer;
	float Shininess;
} Material;

typedef struct MESH_COLLECTION
{
	u32 NumMeshes, TotalVertices, TotalMaterials, *MaterialOffsets, *MaterialCounts, NumTextures, LayerNumTextures[6];
	Texture **Textures[6];
	Material *Materials;
	u8 *Vertices;
	AABB *AABBs;
} MeshCollection;

/* Extract .obj file folder path (without filename) */
/* and pointer for appending relative paths */
static char *get_path(const char *s, char **l, int size)
{
	char *p, *q;
	if(!(p = malloc(size)))
	{
		return NULL;
	}

	strcpy(p, s);
	*l = p;
	for(q = p; *q; ++q)
	{
		if(*q == '/')
		{
			*l = q + 1;
		}
	}

	**l = '\0';
	return p;
}

static u32 next_pow2(u32 x)
{
	return x == 1 ? 1 : 1 << (32 - __builtin_clz(x - 1));
}

static u32 ilog2(u32 x)
{
	return 32 - __builtin_clz(x) - 1;
}

/* linear interpolation */
static float lerp(float s, float e, float t)
{
	return s + (e - s) * t;
}

/* bilinear interpolation */
static float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
{
	return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

static int texture_color(Texture *texture, int size, u8 r, u8 g, u8 b)
{
	int i, bytes;
	texture->Size = size;
	bytes = 4 * size * size;
	if(!(texture->Data = malloc(bytes)))
	{
		return 1;
	}

	for(i = 0; i < bytes; i += 4)
	{
		texture->Data[i + 0] = b;
		texture->Data[i + 1] = g;
		texture->Data[i + 2] = r;
		texture->Data[i + 3] = 255;
	}

	return 0;
}

static int texture_load(char *filename, Texture *texture)
{
	u8 *data;
	int width, height, channels, size;
	if(!(data = stbi_load(filename, &width, &height, &channels, 0)))
	{
		fprintf(stderr, "Failed to load texture \"%s\"\n", filename);
		return 1;
	}

	fprintf(stderr, "Texture channels: %d\n", channels);

	{
		int w, h;
		w = next_pow2(width);
		h = next_pow2(height);
		size = w > h ? w : h;
		if(size > MAX_LAYER_SIZE)
		{
			size = MAX_LAYER_SIZE;
		}

		if(size < MIN_LAYER_SIZE)
		{
			size = MIN_LAYER_SIZE;
		}
	}

	fprintf(stderr, "\tInput Width: %d\n\tInput Height: %d\n\tScaled Size: %d\n", width, height, size);

	texture->Size = size;
	if(!(texture->Data = malloc(4 * size * size)))
	{
		fprintf(stderr, "Failed to allocate memory for scaled texture data\n");
		return 1;
	}

	if(width == size && height == size)
	{
		if(channels == 4)
		{
			for(int i = 0; i < size * size; ++i)
			{
				texture->Data[4 * i + 0] = data[4 * i + 2];
				texture->Data[4 * i + 1] = data[4 * i + 1];
				texture->Data[4 * i + 2] = data[4 * i + 0];
				texture->Data[4 * i + 3] = data[4 * i + 3];
			}

			return 0;
		}
		else if(channels == 3)
		{
			if(!(texture->Data = malloc(4 * size * size)))
			{
				fprintf(stderr, "Failed to allocate memory for scaled texture data\n");
				return 1;
			}

			for(int i = 0; i < size * size; ++i)
			{
				texture->Data[4 * i + 0] = data[3 * i + 2];
				texture->Data[4 * i + 1] = data[3 * i + 1];
				texture->Data[4 * i + 2] = data[3 * i + 0];
				texture->Data[4 * i + 3] = 255;
			}

			return 0;
		}
		else
		{
			return 1;
		}
	}

	if(channels == 4)
	{
		u8 color0[4], color1[4], color2[4], color3[4];
		int x, y, req;
		for(x = 0, y = 0; ; ++x)
		{
			if(x >= size)
			{
				x = 0;
				++y;
				if(y == size)
				{
					break;
				}
			}

			float gx = x / (float)(size) * (width - 1);
			float gy = y / (float)(size) * (height - 1);
			int gxi = (int)gx;
			int gyi = (int)gy;
			int ux, uy;

			ux = gxi >= width ? width - 1 : gxi;
			uy = gyi >= height ? height - 1 : gyi;

			req = uy * width + ux;
			color0[0] = data[4 * req + 0];
			color0[1] = data[4 * req + 1];
			color0[2] = data[4 * req + 2];
			color0[3] = data[4 * req + 3];

			ux = gxi + 1 >= width ? width - 1 : gxi + 1;
			uy = gyi >= height ? height - 1 : gyi;

			req = uy * width + ux;
			color1[0] = data[4 * req + 0];
			color1[1] = data[4 * req + 1];
			color1[2] = data[4 * req + 2];
			color1[3] = data[4 * req + 3];

			ux = gxi >= width ? width - 1 : gxi;
			uy = gyi + 1 >= height ? height - 1 : gyi + 1;

			req = uy * width + ux;
			color2[0] = data[4 * req + 0];
			color2[1] = data[4 * req + 1];
			color2[2] = data[4 * req + 2];
			color2[3] = data[4 * req + 3];

			ux = gxi + 1 >= width ? width - 1 : gxi + 1;
			uy = gyi + 1 >= height ? height - 1 : gyi + 1;

			req = uy * width + ux;
			color3[0] = data[4 * req + 0];
			color3[1] = data[4 * req + 1];
			color3[2] = data[4 * req + 2];
			color3[3] = data[4 * req + 3];

			texture->Data[4 * (y * size + x) + 0] = (uint8_t)blerp(color0[2], color1[2], color2[2], color3[2], gx - gxi, gy - gyi);
			texture->Data[4 * (y * size + x) + 1] = (uint8_t)blerp(color0[1], color1[1], color2[1], color3[1], gx - gxi, gy - gyi);
			texture->Data[4 * (y * size + x) + 2] = (uint8_t)blerp(color0[0], color1[0], color2[0], color3[0], gx - gxi, gy - gyi);
			texture->Data[4 * (y * size + x) + 3] = (uint8_t)blerp(color0[3], color1[3], color2[3], color3[3], gx - gxi, gy - gyi);
		}
	}
	else if(channels == 3)
	{
		u8 color0[3], color1[3], color2[3], color3[3];
		int x, y, req;
		for(x = 0, y = 0; ; ++x)
		{
			if(x >= size)
			{
				x = 0;
				++y;
				if(y == size)
				{
					break;
				}
			}

			float gx = x / (float)(size) * (width - 1);
			float gy = y / (float)(size) * (height - 1);
			int gxi = (int)gx;
			int gyi = (int)gy;
			int ux, uy;

			ux = gxi >= width ? width - 1 : gxi;
			uy = gyi >= height ? height - 1 : gyi;

			req = uy * width + ux;
			color0[0] = data[3 * req + 0];
			color0[1] = data[3 * req + 1];
			color0[2] = data[3 * req + 2];

			ux = gxi + 1 >= width ? width - 1 : gxi + 1;
			uy = gyi >= height ? height - 1 : gyi;

			req = uy * width + ux;
			color1[0] = data[3 * req + 0];
			color1[1] = data[3 * req + 1];
			color1[2] = data[3 * req + 2];

			ux = gxi >= width ? width - 1 : gxi;
			uy = gyi + 1 >= height ? height - 1 : gyi + 1;

			req = uy * width + ux;
			color2[0] = data[3 * req + 0];
			color2[1] = data[3 * req + 1];
			color2[2] = data[3 * req + 2];

			ux = gxi + 1 >= width ? width - 1 : gxi + 1;
			uy = gyi + 1 >= height ? height - 1 : gyi + 1;

			req = uy * width + ux;
			color3[0] = data[3 * req + 0];
			color3[1] = data[3 * req + 1];
			color3[2] = data[3 * req + 2];

			texture->Data[4 * (y * size + x) + 0] = (uint8_t)blerp(color0[2], color1[2], color2[2], color3[2], gx - gxi, gy - gyi);
			texture->Data[4 * (y * size + x) + 1] = (uint8_t)blerp(color0[1], color1[1], color2[1], color3[1], gx - gxi, gy - gyi);
			texture->Data[4 * (y * size + x) + 2] = (uint8_t)blerp(color0[0], color1[0], color2[0], color3[0], gx - gxi, gy - gyi);
			texture->Data[4 * (y * size + x) + 3] = 255;
		}
	}
	else
	{
		return 1;
	}

	return 0;
}

/* Parse a .obj and .mtl file */
int obj_load(const char *filename, Mesh3D *mesh)
{
	int i;
	char *path, *append;
	fprintf(stderr, "Loading OBJ file \"%s\"\n", filename);
	if(!(path = get_path(filename, &append, 256)))
	{
		return 1;
	}

	fprintf(stderr, "Path: %s\n", path);

	{
		u32 data_len;
		char *data;

		/* Read .obj file */
		if(!(data = readfile(filename, &data_len)))
		{
			fprintf(stderr, "Failed to read .obj file\n");
			return 1;
		}

		/* Parse .obj file and associated .mtl file */
		if(tinyobj_parse_obj(path, append, &mesh->attrs, &mesh->shapes, &mesh->num_shapes, &mesh->materials,
			&mesh->num_materials, data, data_len, TINYOBJ_FLAG_TRIANGULATE))
		{
			fprintf(stderr, "Failed to parse .obj file\n");
			return 1;
		}

		free(data);
	}

	*append = '\0';
	fprintf(stderr, ".mtl file: %s\n", path);

	if(!(mesh->textures = malloc(TEXTURES_PER_MATERIAL * mesh->num_materials * sizeof(Texture))))
	{
		fprintf(stderr, "Failed to allocate memory for texture objects\n");
		return 1;
	}

	fputc('\n', stderr);
	for(i = 0; i < mesh->num_materials; ++i)
	{
		fprintf(stderr, "Material #%d\n", i);

		/* Diffuse map */
		mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_DIFFUSE].Data = NULL;
		if(mesh->materials[i].diffuse_texname)
		{
			strcpy(append, mesh->materials[i].diffuse_texname);
			fprintf(stderr, "Diffuse map: %s\n", path);
			if(texture_load(path, &mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_DIFFUSE]))
			{
				fprintf(stderr, "Failed to load diffuse map\n");
				mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_DIFFUSE].Data = NULL;
			}
		}
		else
		{
			fprintf(stderr, "Diffuse map: N/A\n");
		}

		/* Specular */
		mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_SPECULAR].Data = NULL;
		if(mesh->materials[i].specular_texname)
		{
			strcpy(append, mesh->materials[i].specular_texname);
			fprintf(stderr, "Specular map: %s\n", path);
			if(texture_load(path, &mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_SPECULAR]))
			{
				fprintf(stderr, "Failed to load specular map\n");
				mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_SPECULAR].Data = NULL;
			}
		}
		else
		{
			fprintf(stderr, "Specular map: N/A\n");
		}

		/* Emissive map */
		mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_EMISSIVE].Data = NULL;
		if(mesh->materials[i].emissive_texname)
		{
			strcpy(append, mesh->materials[i].emissive_texname);
			fprintf(stderr, "Emissive map: %s\n", path);
			if(texture_load(path, &mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_EMISSIVE]))
			{
				fprintf(stderr, "Failed to load diffuse map\n");
				mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_EMISSIVE].Data = NULL;
			}
		}
		else
		{
			fprintf(stderr, "Emissive map: N/A\n");
		}

		/* Normal map */
		mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_NORMAL].Data = NULL;
		if(mesh->materials[i].bump_texname)
		{
			strcpy(append, mesh->materials[i].bump_texname);
			fprintf(stderr, "Normal map: %s\n", path);
			if(texture_load(path, &mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_NORMAL]))
			{
				fprintf(stderr, "Failed to load normal map\n");
				mesh->textures[TEXTURES_PER_MATERIAL * i + TEXTURE_NORMAL].Data = NULL;
			}
		}
		else
		{
			fprintf(stderr, "Normal map: N/A\n");
		}

		fputc('\n', stderr);
	}

	return 0;
}

u32 pack_gl_int_2_10_10_10_rev(u32 x, u32 y, u32 z, u32 w)
{
	return ((w & 0x03) << 30) | ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10) | (x & 0x3FF);
}

u32 fp_normalize(float normal)
{
	i32 v = normal * 511.0f;
	return ((v < 0) << 9) | (v & 0x1FF);
}

static void pack_32(u8 *dst, u32 val)
{
	dst[0] = (u8)val;
	dst[1] = (u8)(val >> 8);
	dst[2] = (u8)(val >> 16);
	dst[3] = (u8)(val >> 24);
}

static void pack_float(u8 *dst, float val)
{
	memcpy(dst, &val, 4);
}

static void pack_normal(u8 *out, vec3 normal)
{
	pack_32(out, pack_gl_int_2_10_10_10_rev(
		fp_normalize(normal[X]),
		fp_normalize(normal[Y]),
		fp_normalize(normal[Z]), 0));
}

static void vertex(u8 *out, vec3 position, vec3 normal, vec3 tangent, vec3 bitangent, float texture[2])
{
	/* Position */
	pack_float(&out[0], position[X]);
	pack_float(&out[4], position[Y]);
	pack_float(&out[8], position[Z]);

	/* Normal */
	pack_normal(&out[12], normal);

	/* Texture */
	pack_float(&out[16], texture[X]);
	pack_float(&out[20], texture[Y]);

	/* Tangent */
	pack_normal(&out[24], tangent);

	/* Bitangent */
	pack_normal(&out[28], bitangent);
}

/* Convert multiple Mesh3Ds to a MeshCollection */
int mc_convert(MeshCollection *mc, Mesh3D *meshes, int count)
{
	int i, j, f, k, face_offset, num_textures, num_vertices, num_materials, bytes, size, size_id;
	tinyobj_vertex_index_t idx[3];
	float v[3][3], n[3][3], tex[3][2];
	static Texture empty, empty_normal;

	fprintf(stderr, "\nTEXTURES:\n");

	/* White */
	if(texture_color(&empty, 32, 255, 255, 255))
	{
		return 1;
	}

	/* Empty Normal */
	if(texture_color(&empty_normal, 32, 128, 128, 255))
	{
		return 1;
	}

	/* Textures */
	num_textures = 0;
	for(i = 0; i < count; ++i)
	{
		for(j = 0; j < TEXTURES_PER_MATERIAL * meshes[i].num_materials; ++j)
		{
			meshes[i].textures[j].Dupe = NULL;
			if(meshes[i].textures[j].Data)
			{
				for(int v = 0; v < i; ++v)
				{
					for(int w = 0; w < TEXTURES_PER_MATERIAL * meshes[v].num_materials; ++w)
					{
						if(meshes[v].textures[w].Data && meshes[v].textures[w].Size == meshes[i].textures[j].Size)
						{
							int a;
							for(a = 0; a < 1024; ++a)
							{
								int b = rand() % (4 * meshes[i].textures[j].Size * meshes[i].textures[j].Size);
								if(meshes[v].textures[w].Data[b] != meshes[i].textures[j].Data[b])
								{
									break;
								}
							}

							if(a == 1024)
							{
								meshes[i].textures[j].Data = 0;
								meshes[i].textures[j].Dupe = &meshes[v].textures[w];
								goto dupe;
							}
						}
					}
				}

				++num_textures;
dupe:
				;
			}
		}
	}

	fprintf(stderr, "Number of Textures: %d\n", num_textures);
	mc->NumTextures = num_textures;

	for(i = 0; i < TEXTURE_NUM_SIZES; ++i)
	{
		mc->LayerNumTextures[i] = 0;
	}

	mc->LayerNumTextures[0] = 2;
	for(i = 0; i < count; ++i)
	{
		for(j = 0; j < TEXTURES_PER_MATERIAL * meshes[i].num_materials; ++j)
		{
			if(meshes[i].textures[j].Data)
			{
				size = meshes[i].textures[j].Size;
				size_id = SIZE_ID(size);

#ifdef DEBUG
			{
				char buf[128];
				snprintf(buf, sizeof(buf), "dbgtex_%d_%d.bmp", size, mc->LayerNumTextures[size_id]);
				bmp_write(buf, meshes[i].textures[j].Data, size, size);
			}
#endif

				++mc->LayerNumTextures[size_id];
			}
		}
	}

	for(i = 0; i < 6; ++i)
	{
		if(mc->LayerNumTextures[i])
		{
			if(!(mc->Textures[i] = malloc(mc->LayerNumTextures[i] * sizeof(Texture *))))
			{
				return 1;
			}

			mc->LayerNumTextures[i] = 0;
		}
	}

	mc->LayerNumTextures[0] = 2;
	for(i = 0; i < count; ++i)
	{
		for(j = 0; j < TEXTURES_PER_MATERIAL * meshes[i].num_materials; ++j)
		{
			if(meshes[i].textures[j].Data)
			{
				size = meshes[i].textures[j].Size;
				size_id = SIZE_ID(size);
				meshes[i].textures[j].Layer = mc->LayerNumTextures[size_id];
				mc->Textures[size_id][mc->LayerNumTextures[size_id]++] = &meshes[i].textures[j];
			}
		}
	}

	mc->Textures[0][0] = &empty;
	mc->Textures[0][1] = &empty_normal;

	/* Materials */
	fprintf(stderr, "\nMATERIALS:\n");

	mc->TotalMaterials = 0;
	mc->NumMeshes = count;
	if(!(mc->MaterialOffsets = malloc(2 * count * sizeof(u32))))
	{
		fprintf(stderr, "Failed to allocate memory for material offset and counts\n");
		return 1;
	}

	mc->MaterialCounts = mc->MaterialOffsets + count;
	for(i = 0; i < count; ++i)
	{
		num_materials = meshes[i].num_materials;
		mc->MaterialOffsets[i] = mc->TotalMaterials;
		mc->MaterialCounts[i] = num_materials;
		fprintf(stderr, "Material Offset: %d\n", mc->MaterialOffsets[i]);
		fprintf(stderr, "Material Count:  %d\n", mc->MaterialCounts[i]);
		mc->TotalMaterials += num_materials;
	}

	fprintf(stderr, "Total number of materials: %d\n", mc->TotalMaterials);

	if(!(mc->Materials = malloc(mc->TotalMaterials * sizeof(Material))))
	{
		fprintf(stderr, "Failed to allocate memory for materials\n");
		return 1;
	}

	for(i = 0; i < count; ++i)
	{
		for(j = 0; j < meshes[i].num_materials; ++j)
		{
			Material *m = &mc->Materials[mc->MaterialOffsets[i] + j];
			Texture *t;

			m->VertexCount = 0;

			m->ColorDiffuse[0] = (u8)(meshes[i].materials[j].diffuse[0] * 255.0f);
			m->ColorDiffuse[1] = (u8)(meshes[i].materials[j].diffuse[1] * 255.0f);
			m->ColorDiffuse[2] = (u8)(meshes[i].materials[j].diffuse[2] * 255.0f);
			m->ColorDiffuse[3] = 255;

			m->ColorSpecular[0] = (u8)(meshes[i].materials[j].specular[0] * 255.0f);
			m->ColorSpecular[1] = (u8)(meshes[i].materials[j].specular[1] * 255.0f);
			m->ColorSpecular[2] = (u8)(meshes[i].materials[j].specular[2] * 255.0f);
			m->ColorSpecular[3] = 255;

			m->ColorEmissive[0] = (u8)(meshes[i].materials[j].emission[0] * 255.0f);
			m->ColorEmissive[1] = (u8)(meshes[i].materials[j].emission[1] * 255.0f);
			m->ColorEmissive[2] = (u8)(meshes[i].materials[j].emission[2] * 255.0f);
			m->ColorEmissive[3] = 255;

			t = &meshes[i].textures[TEXTURES_PER_MATERIAL * j];

			if(t->Data)
			{
				m->DiffuseSizeID = SIZE_ID(t->Size);
				m->DiffuseLayer = t->Layer;
			}
			else if(t->Dupe)
			{
				m->DiffuseSizeID = SIZE_ID(t->Dupe->Size);
				m->DiffuseLayer = t->Dupe->Layer;
			}
			else
			{
				m->DiffuseSizeID = 0;
				m->DiffuseLayer = 0;
			}

			++t;
			if(t->Data)
			{
				m->SpecularSizeID = SIZE_ID(t->Size);
				m->SpecularLayer = t->Layer;
			}
			else if(t->Dupe)
			{
				m->SpecularSizeID = SIZE_ID(t->Dupe->Size);
				m->SpecularLayer = t->Dupe->Layer;
			}
			else
			{
				m->SpecularSizeID = 0;
				m->SpecularLayer = 0;
			}

			++t;
			if(t->Data)
			{
				m->EmissiveSizeID = SIZE_ID(t->Size);
				m->EmissiveLayer = t->Layer;
			}
			else if(t->Dupe)
			{
				m->EmissiveSizeID = SIZE_ID(t->Dupe->Size);
				m->EmissiveLayer = t->Dupe->Layer;
			}
			else
			{
				m->EmissiveSizeID = 0;
				m->EmissiveLayer = 0;
			}

			++t;
			if(t->Data)
			{
				m->NormalSizeID = SIZE_ID(t->Size);
				m->NormalLayer = t->Layer;
			}
			else if(t->Dupe)
			{
				m->NormalSizeID = SIZE_ID(t->Dupe->Size);
				m->NormalLayer = t->Dupe->Layer;
			}
			else
			{
				m->NormalSizeID = 0;
				m->NormalLayer = 1;
			}

			m->Shininess = meshes[i].materials[j].shininess;
		}
	}

	for(i = 0; i < count; ++i)
	{
		for(j = 0; j < meshes[i].attrs.num_face_num_verts; ++j)
		{
			mc->Materials[mc->MaterialOffsets[i] + meshes[i].attrs.material_ids[j]].VertexCount += 3;
		}
	}

	j = 0;
	for(i = 0; i < (int)mc->TotalMaterials; ++i)
	{
		mc->Materials[i].VertexOffset = j;
		j += mc->Materials[i].VertexCount;
	}

	/* Vertices */
	fprintf(stderr, "\nVERTICES:\n");

	mc->TotalVertices = 0;
	for(i = 0; i < count; ++i)
	{
		num_vertices = 3 * meshes[i].attrs.num_face_num_verts;
		fprintf(stderr, "Mesh Offset: %d\n", mc->TotalVertices);
		fprintf(stderr, "Mesh Count:  %d\n", num_vertices);
		mc->TotalVertices += num_vertices;
	}

	bytes = STRIDE * mc->TotalVertices;
	if(!(mc->Vertices = malloc(bytes)))
	{
		fprintf(stderr, "Failed to allocate memory for vertices\n");
		return 1;
	}

	for(i = 0; i < (int)mc->TotalMaterials; ++i)
	{
		mc->Materials[i].VertexCount = 0;
	}

	if(!(mc->AABBs = malloc(count * sizeof(AABB))))
	{
		fprintf(stderr, "Failed to allocate memory for AABBs\n");
		return 1;
	}

	for(i = 0; i < count; ++i)
	{
		fprintf(stderr, "Converting Mesh %d\n", i);

		mc->AABBs[i].Min[X] = FLT_MAX;
		mc->AABBs[i].Min[Y] = FLT_MAX;
		mc->AABBs[i].Min[Z] = FLT_MAX;

		mc->AABBs[i].Max[X] = -FLT_MAX;
		mc->AABBs[i].Max[Y] = -FLT_MAX;
		mc->AABBs[i].Max[Z] = -FLT_MAX;

		for(face_offset = 0, j = 0; j < meshes[i].attrs.num_face_num_verts; ++j)
		{
			for(f = 0; f < meshes[i].attrs.face_num_verts[j] / 3; ++f)
			{
				idx[0] = meshes[i].attrs.faces[face_offset + 3 * f + 0];
				idx[1] = meshes[i].attrs.faces[face_offset + 3 * f + 1];
				idx[2] = meshes[i].attrs.faces[face_offset + 3 * f + 2];
				for(k = 0; k < 3; ++k)
				{
					v[0][k] = meshes[i].attrs.vertices[3 * idx[0].v_idx + k];
					v[1][k] = meshes[i].attrs.vertices[3 * idx[1].v_idx + k];
					v[2][k] = meshes[i].attrs.vertices[3 * idx[2].v_idx + k];
				}

				if(meshes[i].attrs.num_normals > 0)
				{
					if(idx[0].vn_idx >= 0 && idx[1].vn_idx >= 0 && idx[2].vn_idx >= 0)
					{
						for(k = 0; k < 3; ++k)
						{
							n[0][k] = meshes[i].attrs.normals[3 * idx[0].vn_idx + k];
							n[1][k] = meshes[i].attrs.normals[3 * idx[1].vn_idx + k];
							n[2][k] = meshes[i].attrs.normals[3 * idx[2].vn_idx + k];
						}
					}
					else
					{
						tri_norm(n[0], v[0], v[1], v[2]);
						n[1][0] = n[0][0];
						n[1][1] = n[0][1];
						n[1][2] = n[0][2];
						n[2][0] = n[0][0];
						n[2][1] = n[0][1];
						n[2][2] = n[0][2];
					}
				}
				else
				{
					tri_norm(n[0], v[0], v[1], v[2]);
					n[1][0] = n[0][0];
					n[1][1] = n[0][1];
					n[1][2] = n[0][2];
					n[2][0] = n[0][0];
					n[2][1] = n[0][1];
					n[2][2] = n[0][2];
				}

				/* Texture Coords */
				for(k = 0; k < 3; ++k)
				{
					int tex_offset = 2 * idx[k].vt_idx;
					if(idx[k].vt_idx >= 0 && idx[k].vt_idx < meshes[i].attrs.num_texcoords)
					{
						tex[k][X] = meshes[i].attrs.texcoords[tex_offset + X];
						tex[k][Y] = 1.0f - meshes[i].attrs.texcoords[tex_offset + Y];
					}
					else
					{
						tex[k][X] = 0.0f;
						tex[k][Y] = 0.0f;
					}
				}

				/* For all three vertices of a triangle */
				vec3 tangent, bitangent, edge1, edge2, delta_uv1, delta_uv2;
				float f;

				vec3_sub(edge1, v[1], v[0]);
				vec3_sub(edge2, v[2], v[0]);

				vec2_sub(delta_uv1, tex[1], tex[0]);
				vec2_sub(delta_uv2, tex[2], tex[0]);

				f = 1.0f / (delta_uv1[X] * delta_uv2[Y] - delta_uv2[X] * delta_uv1[Y]);

				tangent[X] = f * (delta_uv2[Y] * edge1[X] - delta_uv1[Y] * edge2[X]);
				tangent[Y] = f * (delta_uv2[Y] * edge1[Y] - delta_uv1[Y] * edge2[Y]);
				tangent[Z] = f * (delta_uv2[Y] * edge1[Z] - delta_uv1[Y] * edge2[Z]);

				bitangent[X] = f * (-delta_uv2[X] * edge1[X] + delta_uv1[X] * edge2[X]);
				bitangent[Y] = f * (-delta_uv2[X] * edge1[Y] + delta_uv1[X] * edge2[Y]);
				bitangent[Z] = f * (-delta_uv2[X] * edge1[Z] + delta_uv1[X] * edge2[Z]);

				vec3_norm(tangent, tangent);
				vec3_norm(bitangent, bitangent);

				for(k = 0; k < 3; ++k)
				{
					int material, offset;
					material = mc->MaterialOffsets[i] + meshes[i].attrs.material_ids[j];
					offset = STRIDE * (mc->Materials[material].VertexOffset + mc->Materials[material].VertexCount++);
					vertex(mc->Vertices + offset, v[k], n[k], tangent, bitangent, tex[k]);

					if(v[k][X] < mc->AABBs[i].Min[X])
					{
						mc->AABBs[i].Min[X] = v[k][X];
					}
					else if(v[k][X] > mc->AABBs[i].Max[X])
					{
						mc->AABBs[i].Max[X] = v[k][X];
					}

					if(v[k][Y] < mc->AABBs[i].Min[Y])
					{
						mc->AABBs[i].Min[Y] = v[k][Y];
					}
					else if(v[k][Y] > mc->AABBs[i].Max[Y])
					{
						mc->AABBs[i].Max[Y] = v[k][Y];
					}

					if(v[k][Z] < mc->AABBs[i].Min[Z])
					{
						mc->AABBs[i].Min[Z] = v[k][Z];
					}
					else if(v[k][Z] > mc->AABBs[i].Max[Z])
					{
						mc->AABBs[i].Max[Z] = v[k][Z];
					}
				}
			}

			face_offset += meshes[i].attrs.face_num_verts[j];
		}
	}

	return 0;
}

/* Write a MeshCollection to a .amf file */
int mc_write(const char *filename, MeshCollection *mc)
{
	static const u8 ident[4] = { 'A', 'M', 'F', 0 };
	FILE *fp;
	u32 i, j, size;
	fprintf(stderr, "\nWriting .amf file \"%s\"\n", filename);
	if(!(fp = fopen(filename, "wb")))
	{
		fprintf(stderr, "Failed to open file \"%s\" for writing\n", filename);
		return 1;
	}

	fwrite(ident, 1, 4, fp);
	fwrite(mc->LayerNumTextures, sizeof(u32), 6, fp);
	for(i = 0; i < 6; ++i)
	{
		fprintf(stderr, "Writing Texture Array Size %d\n", i);
		for(j = 0; j < mc->LayerNumTextures[i]; ++j)
		{
			fprintf(stderr, "\tWriting Texture %d\n", j);
			size = mc->Textures[i][j]->Size;
			fwrite(mc->Textures[i][j]->Data, sizeof(u8), 4 * size * size, fp);
		}
	}

	fprintf(stderr, "Writing Number of Meshes: %d\n", mc->NumMeshes);
	fwrite(&mc->NumMeshes, sizeof(u32), 1, fp);

	fprintf(stderr, "Writing Total Number of Materials: %d\n", mc->TotalMaterials);
	fwrite(&mc->TotalMaterials, sizeof(u32), 1, fp);

	fprintf(stderr, "Writing Total Number of Vertices: %d\n", mc->TotalVertices);
	fwrite(&mc->TotalVertices, sizeof(u32), 1, fp);

	fprintf(stderr, "Writing Material Offset/Count Table\n");
	fwrite(mc->MaterialOffsets, sizeof(u32), 2 * mc->NumMeshes, fp);

	fprintf(stderr, "Writing Materials Table\n");
	fprintf(stderr, "ID   | Offset   | Size     | DColor  | SColor  | EColor  | DSI | SSI | ESI | NSI | DLY | SLY | ELY | NLY | SHINY    |\n");
	for(i = 0; i < mc->TotalMaterials; ++i)
	{
		Material *m = &mc->Materials[i];
		fprintf(stderr, "%4d | %8d | %8d | #%02x%02x%02x | #%02x%02x%02x | #%02x%02x%02x | %3d | %3d | %3d | %3d | %3d | %3d | %3d | %3d | %8.4f |\n",
				i, m->VertexOffset, m->VertexCount,
				m->ColorDiffuse[0], m->ColorDiffuse[1], m->ColorDiffuse[2],
				m->ColorSpecular[0], m->ColorSpecular[1], m->ColorSpecular[2],
				m->ColorEmissive[0], m->ColorEmissive[1], m->ColorEmissive[2],
				m->DiffuseSizeID, m->SpecularSizeID, m->EmissiveSizeID, m->NormalSizeID,
				m->DiffuseLayer, m->SpecularLayer, m->EmissiveLayer, m->NormalLayer,
				m->Shininess);

		fwrite(&m->VertexOffset, sizeof(u32), 1, fp);
		fwrite(&m->VertexCount, sizeof(u32), 1, fp);

		fwrite(&m->ColorDiffuse, sizeof(u8), 4, fp);
		fwrite(&m->ColorSpecular, sizeof(u8), 4, fp);
		fwrite(&m->ColorEmissive, sizeof(u8), 4, fp);

		fwrite(&m->DiffuseSizeID, sizeof(u32), 1, fp);
		fwrite(&m->SpecularSizeID, sizeof(u32), 1, fp);
		fwrite(&m->EmissiveSizeID, sizeof(u32), 1, fp);
		fwrite(&m->NormalSizeID, sizeof(u32), 1, fp);

		fwrite(&m->DiffuseLayer, sizeof(u32), 1, fp);
		fwrite(&m->SpecularLayer, sizeof(u32), 1, fp);
		fwrite(&m->EmissiveLayer, sizeof(u32), 1, fp);
		fwrite(&m->NormalLayer, sizeof(u32), 1, fp);

		fwrite(&m->Shininess, sizeof(float), 1, fp);
	}

	fprintf(stderr, "Writing Vertices:\n");
	fwrite(mc->Vertices, 1, STRIDE * mc->TotalVertices, fp);

	fprintf(stderr, "Writing AABBs:\n");
	fwrite(mc->AABBs, 1, mc->NumMeshes * sizeof(AABB), fp);

	fclose(fp);
	return 0;
}

int main(int argc, char **argv)
{
	int i, num_meshes;
	Mesh3D *meshes;
	MeshCollection mc;

	assert(sizeof(u32) == 4);
	assert(sizeof(float) == 4);

	if(argc < 3)
	{
		fprintf(stderr, "Usage: ./model_convert \"out\" \"in-0\" ...\n");
		return 1;
	}

	num_meshes = argc - 2;
	fprintf(stderr, "Number of Meshes: %d\n\n", num_meshes);
	if(!(meshes = malloc(num_meshes * sizeof(Mesh3D))))
	{
		fprintf(stderr, "Failed to allocate memory for %d meshes\n", num_meshes);
		return 1;
	}

	for(i = 0; i < num_meshes; ++i)
	{
		if(obj_load(argv[i + 2], &meshes[i]))
		{
			return 1;
		}
	}

	mc_convert(&mc, meshes, num_meshes);
	mc_write(argv[1], &mc);
	return 0;
}

