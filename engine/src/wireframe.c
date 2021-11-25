#include "wireframe.h"
#include "opengl.h"
#include "shader.h"
#include "shaders.h"
#include "uniform.h"
#include "debug.h"
#include "strings.h"

#define MAX_CUBES (10 * 1024)

static u32 _index;
static GLuint _shader, _vbo[3], _vao;
static GLint _uniform_color, _uniform_perspective;

static float _vertices[] =
{
	 0.5,  0.5,  0.5,
	-0.5,  0.5,  0.5,
	 0.5,  0.5, -0.5,
	-0.5,  0.5, -0.5,

	 0.5, -0.5,  0.5,
	-0.5, -0.5,  0.5,
	 0.5, -0.5, -0.5,
	-0.5, -0.5, -0.5,
};

static u8 _indices[] =
{
	0, 1,
	0, 2,
	1, 3,
	2, 3,

	0, 4,
	1, 5,
	2, 6,
	3, 7,

	4, 5,
	4, 6,
	5, 7,
	6, 7
};

static mat4 _cube_models[MAX_CUBES];

int wireframe_init(void)
{
	if(!(_shader = shader_load_string_vf(wireframe_vs, wireframe_fs)))
	{
		return 1;
	}

	_uniform_perspective = uniform_get(_shader, s_uPerspective);
	_uniform_color = uniform_get(_shader, s_uColor);

	GL_CHECK(glGenVertexArrays(1, &_vao));
	GL_CHECK(glGenBuffers(3, _vbo));

	GL_CHECK(glBindVertexArray(_vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[1]));

	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW));

	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW));
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]));
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void *)0));
	GL_CHECK(glVertexAttribDivisor(1, 1));

	GL_CHECK(glEnableVertexAttribArray(2));
	GL_CHECK(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void *)(1 * sizeof(vec4))));
	GL_CHECK(glVertexAttribDivisor(2, 1));

	GL_CHECK(glEnableVertexAttribArray(3));
	GL_CHECK(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void *)(2 * sizeof(vec4))));
	GL_CHECK(glVertexAttribDivisor(3, 1));

	GL_CHECK(glEnableVertexAttribArray(4));
	GL_CHECK(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(vec4), (void *)(3 * sizeof(vec4))));
	GL_CHECK(glVertexAttribDivisor(4, 1));

	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, MAX_CUBES * sizeof(mat4), _cube_models, GL_STREAM_DRAW));
	return 0;
}

void wireframe_use(mat4 perspective)
{
	GL_CHECK(glUseProgram(_shader));
	GL_CHECK(glUniformMatrix4fv(_uniform_perspective, 1, GL_FALSE, &perspective[0][0]));
}

void wireframe_aabb(AABB *box)
{
	mat4 *model = _cube_models + _index;
	vec3 size;
	size[X] = box->Max[X] - box->Min[X];
	size[Y] = box->Max[Y] - box->Min[Y];
	size[Z] = box->Max[Z] - box->Min[Z];
	mat4_translate(*model,
			box->Min[X] + size[X] / 2.0f,
			box->Min[Y] + size[Y] / 2.0f,
			box->Min[Z] + size[Z] / 2.0f);

	mat4_scale(*model, *model, size[X], size[Y], size[Z]);
	++_index;
}

void wireframe_render(const vec4 color)
{
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]));
	GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(_index * sizeof(mat4)), _cube_models));
	GL_CHECK(glUniform4f(_uniform_color, color[0], color[1], color[2], color[3]));
	GL_CHECK(glBindVertexArray(_vao));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[1]));
	GL_CHECK(glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_BYTE, NULL, (GLsizei)_index));
	_index = 0;
}

void wireframe_destroy(void)
{
	GL_CHECK(glDeleteBuffers(3, _vbo));
	GL_CHECK(glDeleteVertexArrays(1, &_vao));
}

