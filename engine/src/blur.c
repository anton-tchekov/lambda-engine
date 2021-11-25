#include "blur.h"
#include "shader.h"
#include "shaders.h"
#include "debug.h"
#include "log.h"
#include "uniform.h"
#include "strings.h"
#include "internal.h"

static GLint _u_horizontal, _u_sampler;
static GLuint _vbo, _vao, _shader5;

static const float _quad[] =
{
	-1.0f, -1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 1.0f, 1.0f
};

int gblur_init(void)
{
	GLuint shader;
	log_debug("Loading blur shaders");
	if(!(shader = shader_load_string_vf(blur_vs, blur_fs)))
	{
		return 1;
	}

	_shader5 = shader;
	GL_CHECK(glUseProgram(shader));
	_u_horizontal = uniform_get(shader, s_uHorizontal);
	_u_sampler = uniform_get(shader, s_uSampler);

	GL_CHECK(glGenVertexArrays(1, &_vao));
	GL_CHECK(glGenBuffers(1, &_vbo));
	GL_CHECK(glBindVertexArray(_vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(_quad), &_quad, GL_STATIC_DRAW));

	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0));
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float))));

	return 0;
}

void gblur_destroy(void)
{
	GL_CHECK(glDeleteProgram(_shader5));
	GL_CHECK(glDeleteVertexArrays(1, &_vao));
	GL_CHECK(glDeleteBuffers(1, &_vbo));
}

void gblur5(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;

glDisable(GL_DEPTH_TEST);
glDisable(GL_CULL_FACE);

	GL_CHECK(glUseProgram(_shader5));
	GL_CHECK(glBindVertexArray(_vao));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->BlurFBO));
	GL_CHECK(glUniform1i(_u_sampler, TU_DRAW));
	GL_CHECK(glUniform1i(_u_horizontal, 0));
	GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->DrawFBO));
	GL_CHECK(glUniform1i(_u_sampler, TU_BLUR));
	GL_CHECK(glUniform1i(_u_horizontal, 1));
	GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

glEnable(GL_DEPTH_TEST);
glEnable(GL_CULL_FACE);
}

