#ifndef __SHADER_H__
#define __SHADER_H__

#include "types.h"
#include "opengl.h"

GLuint shader_compile_file(const char *shader_filename, GLenum shader_type);
GLuint shader_compile_string(const char *shader_src, GLenum shader_type);
GLuint shader_link_vf(GLuint vertex, GLuint fragment);
GLuint shader_link_vgf(GLuint vertex, GLuint geometry, GLuint fragment);
void shader_delete_vf(GLuint vertex, GLuint fragment);
void shader_delete_vgf(GLuint vertex, GLuint geometry, GLuint fragment);
GLuint shader_load_string_vf(
		const char *vertex_filename,
		const char *fragment_filename);
GLuint shader_load_string_vgf(
		const char *vertex_src,
		const char *geometry_src,
		const char *fragment_src);
GLuint shader_load_file_vf(
		const char *vertex_filename,
		const char *fragment_filename);
GLuint shader_load_file_vgf(
		const char *vertex_filename,
		const char *geometry_filename,
		const char *fragment_filename);

#endif

