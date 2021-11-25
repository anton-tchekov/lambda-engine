#include "csm.h"
#include "opengl.h"
#include "internal.h"
#include "debug.h"

void csm_render(mat4 projection)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glUseProgram(renderer->ShaderShadowMap));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->ShadowFBO));
	GL_CHECK(glViewport(0, 0, CSM_SIZE, CSM_SIZE));
	GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
	scene_render_shadow_map(projection);
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
