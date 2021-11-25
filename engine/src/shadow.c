#include "shadow.h"
#include "internal.h"

#define BIAS                0.0001f
#define EXTRUDE_DISTANCE    50.0f
#define MAX_PROFILE_INDICES (10 * 1024)

static u32 profile_indices[MAX_PROFILE_INDICES];

void add_bias(vec3 light_pos, vec3 in)
{
	vec3 bias;
	vec3_sub(bias, in, light_pos);
	vec3_scale(bias, bias, BIAS);
	vec3_add(in, in, bias);
}

void shadow_volume_generate(vec3 light_pos, mat4 model, u32 id)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	ShadowCollection *sc = renderer->Shadows;
	vec3 *transformed = sc->Transformed;
	vec3 *vertices = sc->Vertices + sc->VertexOffsets[id];
	u32 *indices = sc->Indices + sc->IndexOffsets[id];
	ShadowEdge *edges = sc->Edges + sc->EdgeOffsets[id];

	vec3 *shadow_vertices = renderer->ShadowVertices + renderer->ShadowNumVertices;
	u32 *shadow_indices = renderer->ShadowIndices + renderer->ShadowNumIndices;

	u32 i, idx = renderer->ShadowNumVertices;
	u32 num_vertices = sc->VertexCounts[id];
	u32 num_indices = sc->IndexCounts[id];
	u32 num_edges = sc->EdgeCounts[id];

#ifdef ID_V_I_E
	printf("S_ID = %d\n", id);
	printf("VO = %d\n", sc->VertexOffsets[id]);
	printf("VC = %d\n", sc->VertexCounts[id]);

	printf("IO = %d\n", sc->IndexOffsets[id]);
	printf("IC = %d\n", sc->IndexCounts[id]);

	printf("EO = %d\n", sc->EdgeOffsets[id]);
	printf("EC = %d\n", sc->EdgeCounts[id]);
#endif

	u32 profile_idx, vertices_idx, indices_idx;
	vec3 tmp;

	for(i = 0; i < num_vertices; ++i)
	{
		mat4_mul_vec3(transformed[i], model, vertices[i]);
	}

	profile_idx = 0;
	vertices_idx = 0;
	indices_idx = 0;

	/* Find the objects profile */
	for(i = 0; i < num_edges; ++i)
	{
		u32 i0_0 = edges[i].F0 * 3 + 0;
		u32 i1_0 = edges[i].F0 * 3 + 1;
		u32 i2_0 = edges[i].F0 * 3 + 2;

		u32 i0_1 = edges[i].F1 * 3 + 0;
		u32 i1_1 = edges[i].F1 * 3 + 1;
		u32 i2_1 = edges[i].F1 * 3 + 2;

		u32 V0_0 = indices[i0_0];
		u32 V1_0 = indices[i1_0];
		u32 v2_0 = indices[i2_0];

		u32 V0_1 = indices[i0_1];
		u32 V1_1 = indices[i1_1];
		u32 v2_1 = indices[i2_1];

		vec3 p0_0 = { transformed[V0_0][X], transformed[V0_0][Y], transformed[V0_0][Z] };
		vec3 p1_0 = { transformed[V1_0][X], transformed[V1_0][Y], transformed[V1_0][Z] };
		vec3 p2_0 = { transformed[v2_0][X], transformed[v2_0][Y], transformed[v2_0][Z] };

		vec3 p0_1 = { transformed[V0_1][X], transformed[V0_1][Y], transformed[V0_1][Z] };
		vec3 p1_1 = { transformed[V1_1][X], transformed[V1_1][Y], transformed[V1_1][Z] };
		vec3 p2_1 = { transformed[v2_1][X], transformed[v2_1][Y], transformed[v2_1][Z] };

		vec3 v_0;
		v_0[X] = (p0_0[X] + p1_0[X] + p2_0[X]) * (1.0f / 3.0f) - light_pos[X];
		v_0[Y] = (p0_0[Y] + p1_0[Y] + p2_0[Y]) * (1.0f / 3.0f) - light_pos[Y];
		v_0[Z] = (p0_0[Z] + p1_0[Z] + p2_0[Z]) * (1.0f / 3.0f) - light_pos[Z];
		vec3_norm(v_0, v_0);

		vec3 v_1;
		v_1[X] = (p0_1[X] + p1_1[X] + p2_1[X]) * (1.0f / 3.0f) - light_pos[X];
		v_1[Y] = (p0_1[Y] + p1_1[Y] + p2_1[Y]) * (1.0f / 3.0f) - light_pos[Y];
		v_1[Z] = (p0_1[Z] + p1_1[Z] + p2_1[Z]) * (1.0f / 3.0f) - light_pos[Z];
		vec3_norm(v_1, v_1);

		vec3 n_0, t2;
		vec3_sub(t2, p1_0, p0_0);
		vec3_sub(tmp, p2_0, p0_0);
		vec3_cross(n_0, t2, tmp);
		vec3_norm(n_0, n_0);

		vec3 n_1;
		vec3_sub(t2, p1_1, p0_1);
		vec3_sub(tmp, p2_1, p0_1);
		vec3_cross(n_1, t2, tmp);
		vec3_norm(n_1, n_1);

		double a, b;
		a = vec3_dot(v_0, n_0);
		b = vec3_dot(v_1, n_1);
		if(a * b < 0)
		{
			if(a < 0)
			{
				profile_indices[profile_idx++] = edges[i].V1;
				profile_indices[profile_idx++] = edges[i].V0;
			}
			else
			{
				profile_indices[profile_idx++] = edges[i].V0;
				profile_indices[profile_idx++] = edges[i].V1;
			}
		}
	}

	/* Extrude the profiles edges */
	for(i = 0; i < profile_idx; i += 2)
	{
		u32 i0 = profile_indices[i];
		u32 i1 = profile_indices[i + 1];

		vec3 p0 = { transformed[i0][X], transformed[i0][Y], transformed[i0][Z] };
		vec3 p1 = { transformed[i1][X], transformed[i1][Y], transformed[i1][Z] };
		vec3 l0, l1, ep0, ep1;
		vec3_sub(l0, p0, light_pos);
		vec3_norm(l0, l0);

		vec3_sub(l1, p1, light_pos);
		vec3_norm(l1, l1);

		add_bias(light_pos, p0);
		vec3_dup(shadow_vertices[vertices_idx++], p0);

		vec3_scale(ep0, l0, EXTRUDE_DISTANCE);
		vec3_add(ep0, ep0, p0);

		add_bias(light_pos, ep0);
		vec3_dup(shadow_vertices[vertices_idx++], ep0);

		vec3_scale(ep1, l1, EXTRUDE_DISTANCE);
		vec3_add(ep1, ep1, p1);

		add_bias(light_pos, ep1);
		vec3_dup(shadow_vertices[vertices_idx++], ep1);

		add_bias(light_pos, p1);
		vec3_dup(shadow_vertices[vertices_idx++], p1);

		shadow_indices[indices_idx++] = idx + 0;
		shadow_indices[indices_idx++] = idx + 1;
		shadow_indices[indices_idx++] = idx + 2;

		shadow_indices[indices_idx++] = idx + 0;
		shadow_indices[indices_idx++] = idx + 2;
		shadow_indices[indices_idx++] = idx + 3;
		idx += 4;
	}

	/* Build the near and the far caps */
	for(i = 0; i < num_indices; i += 3)
	{
		vec3 p0 = { transformed[indices[i + 0]][X], transformed[indices[i + 0]][Y], transformed[indices[i + 0]][Z] };
		vec3 p1 = { transformed[indices[i + 1]][X], transformed[indices[i + 1]][Y], transformed[indices[i + 1]][Z] };
		vec3 p2 = { transformed[indices[i + 2]][X], transformed[indices[i + 2]][Y], transformed[indices[i + 2]][Z] };

		vec3 l;
		l[X] = (p0[X] + p1[X] + p2[X]) * (1.0f / 3.0f) - light_pos[X];
		l[Y] = (p0[Y] + p1[Y] + p2[Y]) * (1.0f / 3.0f) - light_pos[Y];
		l[Z] = (p0[Z] + p1[Z] + p2[Z]) * (1.0f / 3.0f) - light_pos[Z];
		vec3_norm(l, l);

		vec3 n, t2;
		vec3_sub(t2, p1, p0);
		vec3_sub(tmp, p2, p0);
		vec3_cross(n, t2, tmp);
		vec3_norm(n, n);

		if(vec3_dot(l, n) >= 0.0f)
		{
			vec3_sub(tmp, p0, light_pos);
			vec3_norm(tmp, tmp);
			vec3_scale(tmp, tmp, EXTRUDE_DISTANCE);
			vec3_add(p0, p0, tmp);

			vec3_sub(tmp, p1, light_pos);
			vec3_norm(tmp, tmp);
			vec3_scale(tmp, tmp, EXTRUDE_DISTANCE);
			vec3_add(p1, p1, tmp);

			vec3_sub(tmp, p2, light_pos);
			vec3_norm(tmp, tmp);
			vec3_scale(tmp, tmp, EXTRUDE_DISTANCE);
			vec3_add(p2, p2, tmp);
		}

		add_bias(light_pos, p0);
		vec3_dup(shadow_vertices[vertices_idx++], p0);
		shadow_indices[indices_idx++] = idx++;

		add_bias(light_pos, p1);
		vec3_dup(shadow_vertices[vertices_idx++], p1);
		shadow_indices[indices_idx++] = idx++;

		add_bias(light_pos, p2);
		vec3_dup(shadow_vertices[vertices_idx++], p2);
		shadow_indices[indices_idx++] = idx++;
	}

	renderer->ShadowNumVertices += vertices_idx;
	renderer->ShadowNumIndices += indices_idx;
}

