#include "shader.h"
#include "debug.h"
#include <stdlib.h>

#ifdef SHADER_COMPILE_FILE_USED
#include "readfile.h"
#endif

static u32 _shader_link_result(GLuint program);
static u32 _shader_compile_result(GLuint shader);
static u32 _compiler_info_log(GLuint shader);
static u32 _linker_info_log(GLuint program);

/* #define SHADER_DEBUG */

#ifdef SHADER_COMPILE_FILE_USED
static const char *_shader_name(GLenum shader_type);
#endif

#ifdef SHADER_COMPILE_FILE_USED
GLuint shader_compile_file(const char *shader_filename, GLenum shader_type)
{
	GLuint shader;
	u32 length;
	char *shader_src;
#ifdef SHADER_DEBUG
	log_debug("Reading %s shader file \"%s\"", _shader_name(shader_type), shader_filename);
#endif
	if(!(shader_src = readfile(shader_filename, &length)))
	{
#ifdef SHADER_DEBUG
		log_error("Failed to read shader file");
#endif
		return 0;
	}

	shader = shader_compile_string(shader_src, shader_type);
	free(shader_src);
	return shader;
}
#endif

GLuint shader_compile_string(const char *shader_src, GLenum shader_type)
{
	GLuint shader;
#ifdef SHADER_DEBUG
	log_debug("Creating shader");
#endif
	GL_CHECK(shader = glCreateShader(shader_type));
	GL_CHECK(glShaderSource(shader, 1, (const GLchar *const *)&shader_src, NULL));
#ifdef SHADER_DEBUG
	log_debug("Compiling shader");
#endif
	GL_CHECK(glCompileShader(shader));
	if(_shader_compile_result(shader))
	{
#ifdef SHADER_DEBUG
		log_debug("Deleting shader");
#endif
		GL_CHECK(glDeleteShader(shader));
		return 0;
	}

	return shader;
}

GLuint shader_link_vf(GLuint vertex, GLuint fragment)
{
	GLuint program;
#ifdef SHADER_DEBUG
	log_debug("Creating shader program");
#endif
	GL_CHECK(program = glCreateProgram());
#ifdef SHADER_DEBUG
	log_debug("Attaching vertex (%d) and fragment (%d) shader to program (%d)", vertex, fragment, program);
#endif
	GL_CHECK(glAttachShader(program, vertex));
	GL_CHECK(glAttachShader(program, fragment));
#ifdef SHADER_DEBUG
	log_debug("Linking program");
#endif
	GL_CHECK(glLinkProgram(program));
	if(_shader_link_result(program))
	{
#ifdef SHADER_DEBUG
		log_debug("Deleting program");
#endif
		GL_CHECK(glDeleteProgram(program));
		return 0;
	}

#ifdef SHADER_DEBUG
	log_debug("Detaching shaders");
#endif
	GL_CHECK(glDetachShader(program, vertex));
	GL_CHECK(glDetachShader(program, fragment));
	return program;
}

GLuint shader_link_vgf(GLuint vertex, GLuint geometry, GLuint fragment)
{
	GLuint program;
#ifdef SHADER_DEBUG
	log_debug("Linking vertex, geometry and fragment shader");
#endif
	GL_CHECK(program = glCreateProgram());
#ifdef SHADER_DEBUG
	log_debug("Attaching vertex (%d), geometry (%d) and fragment (%d) shader to program (%d)",
			vertex, geometry, fragment, program);
#endif
	GL_CHECK(glAttachShader(program, vertex));
	GL_CHECK(glAttachShader(program, geometry));
	GL_CHECK(glAttachShader(program, fragment));
#ifdef SHADER_DEBUG
	log_debug("Linking program");
#endif
	GL_CHECK(glLinkProgram(program));
	if(_shader_link_result(program))
	{
#ifdef SHADER_DEBUG
		log_debug("Deleting program");
#endif
		GL_CHECK(glDeleteProgram(program));
		return 0;
	}

#ifdef SHADER_DEBUG
	log_debug("Detaching shaders");
#endif
	GL_CHECK(glDetachShader(program, vertex));
	GL_CHECK(glDetachShader(program, geometry));
	GL_CHECK(glDetachShader(program, fragment));
	return program;
}

void shader_delete_vf(GLuint vertex, GLuint fragment)
{
#ifdef SHADER_DEBUG
	log_debug("Deleting shaders");
#endif
	GL_CHECK(glDeleteShader(vertex));
	GL_CHECK(glDeleteShader(fragment));
}

void shader_delete_vgf(GLuint vertex, GLuint geometry, GLuint fragment)
{
#ifdef SHADER_DEBUG
	log_debug("Deleting shaders");
#endif
	GL_CHECK(glDeleteShader(vertex));
	GL_CHECK(glDeleteShader(geometry));
	GL_CHECK(glDeleteShader(fragment));
}

GLuint shader_load_string_vf(
		const char *vertex_filename,
		const char *fragment_filename)
{
	GLuint program, vertex, fragment;
#ifdef SHADER_DEBUG
	log_debug("Loading shader program");
#endif
	if(!(vertex = shader_compile_string(vertex_filename, GL_VERTEX_SHADER)))
	{
		return 0;
	}

	if(!(fragment = shader_compile_string(fragment_filename, GL_FRAGMENT_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		return 0;
	}

	program = shader_link_vf(vertex, fragment);
	shader_delete_vf(vertex, fragment);
	return program;
}

GLuint shader_load_string_vgf(
		const char *vertex_src,
		const char *geometry_src,
		const char *fragment_src)
{
	GLuint program, vertex, geometry, fragment;
#ifdef SHADER_DEBUG
	log_debug("Loading shader program");
#endif
	if(!(vertex = shader_compile_string(vertex_src, GL_VERTEX_SHADER)))
	{
		return 0;
	}

	if(!(geometry = shader_compile_string(geometry_src, GL_GEOMETRY_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		return 0;
	}

	if(!(fragment = shader_compile_string(fragment_src, GL_FRAGMENT_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		GL_CHECK(glDeleteShader(geometry));
		return 0;
	}

	program = shader_link_vgf(vertex, geometry, fragment);
	shader_delete_vgf(vertex, geometry, fragment);
	return program;
}

#ifdef SHADER_COMPILE_FILE_USED
GLuint shader_load_file_vf(
		const char *vertex_filename,
		const char *fragment_filename)
{
	GLuint program, vertex, fragment;
#ifdef SHADER_DEBUG
	log_debug("Loading shader program");
#endif
	if(!(vertex = shader_compile_file(vertex_filename, GL_VERTEX_SHADER)))
	{
		return 0;
	}

	if(!(fragment = shader_compile_file(fragment_filename, GL_FRAGMENT_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		return 0;
	}

	program = shader_link_vf(vertex, fragment);
	shader_delete_vf(vertex, fragment);
	return program;
}

GLuint shader_load_file_vgf(
		const char *vertex_filename,
		const char *geometry_filename,
		const char *fragment_filename)
{
	GLuint program, vertex, geometry, fragment;
#ifdef SHADER_DEBUG
	log_debug("Loading shader program");
#endif
	if(!(vertex = shader_compile_file(vertex_filename, GL_VERTEX_SHADER)))
	{
		return 0;
	}

	if(!(geometry = shader_compile_file(geometry_filename, GL_GEOMETRY_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		return 0;
	}

	if(!(fragment = shader_compile_file(fragment_filename, GL_FRAGMENT_SHADER)))
	{
		GL_CHECK(glDeleteShader(vertex));
		GL_CHECK(glDeleteShader(geometry));
		return 0;
	}

	program = shader_link_vgf(vertex, geometry, fragment);
	shader_delete_vgf(vertex, geometry, fragment);
	return program;
}
#endif

static u32 _shader_link_result(GLuint program)
{
	int rv;
#ifdef SHADER_DEBUG
	log_debug("Checking link results");
#endif
	GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &rv));

#ifdef DEBUG
	if(_linker_info_log(program))
	{
		return 1;
	}

	if(!rv)
	{
		log_error("Failed to compile shader");
		return 1;
	}

#else
	if(!rv)
	{
		log_error("Failed to link shader program");
		_linker_info_log(program);
		return 1;
	}
#endif

	return 0;
}

static u32 _shader_compile_result(GLuint shader)
{
	int rv;
#ifdef SHADER_DEBUG
	log_debug("Checking compile results");
#endif
	GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &rv));

#ifdef DEBUG
	if(_compiler_info_log(shader))
	{
		return 1;
	}

	if(!rv)
	{
		log_error("Failed to compile shader");
		return 1;
	}

#else
	if(!rv)
	{
		log_error("Failed to compile shader");
		_compiler_info_log(shader);
		return 1;
	}
#endif

	return 0;
}

static u32 _compiler_info_log(GLuint shader)
{
	int rv;
	char *info_log;
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &rv));
	if(rv <= 0)
	{
		return 0;
	}

	if(!(info_log = malloc((size_t)rv)))
	{
		log_error("Failed to allocate memory for the info log");
		return 1;
	}

	GL_CHECK(glGetShaderInfoLog(shader, rv, NULL, info_log));
	log_info("Compiler messages:\n%s", info_log);
	free(info_log);
	return 0;
}

static u32 _linker_info_log(GLuint program)
{
	int rv;
	char *info_log;
	GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &rv));
	if(rv <= 0)
	{
		return 0;
	}

	if(!(info_log = malloc((size_t)rv)))
	{
		log_error("Failed to allocate memory for the info log");
		return 1;
	}

	GL_CHECK(glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log));
	log_info("Linker messages:\n%s", info_log);
	free(info_log);
	return 0;
}

#ifdef SHADER_COMPILE_FILE_USED
static const char *_shader_name(GLenum shader_type)
{
	static const char
		*shader_vertex_name = "vertex",
		*shader_geometry_name = "geometry",
		*shader_fragment_name = "fragment",
		*shader_unknown_name = "unknown";

	switch(shader_type)
	{
	case GL_VERTEX_SHADER:
		return shader_vertex_name;

	case GL_GEOMETRY_SHADER:
		return shader_geometry_name;

	case GL_FRAGMENT_SHADER:
		return shader_fragment_name;

	default:
		return shader_unknown_name;
	}
}
#endif
