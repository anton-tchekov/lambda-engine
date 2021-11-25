#include "skybox.h"
#include "internal.h"
#include "log.h"
#include "debug.h"
#include "shader.h"
#include "shaders.h"
#include "uniform.h"
#include "cubemap.h"
#include "strings.h"

/* #define SKYBOX_DEBUG */

int skybox_init(void)
{
	static const float cube_vertices[] =
	{
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0
	};

	static const u8 cube_indices[] =
	{
		0, 2, 1,
		2, 0, 3,
		1, 6, 5,
		6, 1, 2,
		7, 5, 6,
		5, 7, 4,
		4, 3, 0,
		3, 4, 7,
		4, 1, 5,
		1, 4, 0,
		3, 6, 2,
		6, 3, 7
	};

	GLint i;
	Skybox *skybox = &GameEngine.Graphics.Skybox;

	log_debug("Initializing skybox");
	if(!(skybox->Shader = shader_load_string_vf(skybox_vs, skybox_fs)))
	{
		return 1;
	}

#ifdef SKYBOX_DEBUG
	log_debug("Initializing skybox VAO, VBO and IBO");
#endif
	GL_CHECK(glGenVertexArrays(1, &skybox->VAO));
	GL_CHECK(glGenBuffers(2, skybox->VBO));
	GL_CHECK(glBindVertexArray(skybox->VAO));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, skybox->VBO[0]));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW));

	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->VBO[1]));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW));

	GL_CHECK(glUseProgram(skybox->Shader));
#ifdef SKYBOX_DEBUG
	log_debug("Loading skybox uniforms");
#endif
	GL_CHECK(skybox->Uniform_Perspective = glGetUniformLocation(skybox->Shader, s_uPerspective));
	i = uniform_get(skybox->Shader, s_uCubemap);
	GL_CHECK(glUniform1i(i, TU_SKYBOX));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	return 0;
}

void skybox_destroy(void)
{
	Skybox *skybox = &GameEngine.Graphics.Skybox;
	log_debug("Destroying skybox renderer");
	GL_CHECK(glDeleteVertexArrays(1, &skybox->VAO));
	GL_CHECK(glDeleteBuffers(2, skybox->VBO));
	GL_CHECK(glDeleteProgram(skybox->Shader));
}

void skybox_bind_cubemap(Cubemap cubemap)
{
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_SKYBOX));
	GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap));
}

void skybox_render(mat4 perspective)
{
	Skybox *skybox = &GameEngine.Graphics.Skybox;
	glDepthFunc(GL_LEQUAL);
	GL_CHECK(glUseProgram(skybox->Shader));
	GL_CHECK(glUniformMatrix4fv(skybox->Uniform_Perspective, 1, GL_FALSE, &perspective[0][0]));
	GL_CHECK(glBindVertexArray(skybox->VAO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox->VBO[1]));
	GL_CHECK(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0));
}

