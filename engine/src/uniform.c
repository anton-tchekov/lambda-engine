#include "uniform.h"
#include "debug.h"
#include "log.h"

GLint uniform_get(GLuint program, const GLchar *name)
{
	GLint uniform;
	GL_CHECK(uniform = glGetUniformLocation(program, name));
	if(uniform < 0)
	{
		log_error("Invalid uniform \"%s\"", name);
	}

	return uniform;
}

