#include "opengl.h"
#include "internal.h"

void glBindTexture(GLenum target, GLuint texture)
{
	++GameEngine.Graphics.TEX_CT;
	glad_glBindTexture(target, texture);
}

void glBindBuffer(GLenum target, GLuint buffer)
{
	++GameEngine.Graphics.VBO_CT;
	glad_glBindBuffer(target, buffer);
}

void glBindVertexArray(GLuint array)
{
	++GameEngine.Graphics.VAO_CT;
	glad_glBindVertexArray(array);
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	++GameEngine.Graphics.DRW_CT;
	GameEngine.Graphics.VTX_CT += (u32)count;
	glad_glDrawArrays(mode, first, count);
}

void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	++GameEngine.Graphics.DRW_CT;
	GameEngine.Graphics.VTX_CT += (u32)count;
	glad_glDrawElements(mode, count, type, indices);
}

void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
	++GameEngine.Graphics.DRW_CT;
	GameEngine.Graphics.VTX_CT += (u32)(count * instancecount);
	glad_glDrawArraysInstanced(mode, first, count, instancecount);
}

void glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	++GameEngine.Graphics.FBO_CT;
	glad_glBindFramebuffer(target, framebuffer);
}

