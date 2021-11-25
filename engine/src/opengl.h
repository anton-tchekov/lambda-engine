#ifndef __OPENGL_H__
#define __OPENGL_H__

#include "lib/glad.h"

#ifdef __WIN32

#include "lib/glfw3.h"

#else

#include <GLFW/glfw3.h>

#endif

/* Wrapper functions for performace analysis */
void glBindTexture(GLenum target, GLuint texture);
void glBindBuffer(GLenum target, GLuint buffer);
void glBindVertexArray(GLuint array);
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);
void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
void glBindFramebuffer(GLenum target, GLuint framebuffer);

#endif

