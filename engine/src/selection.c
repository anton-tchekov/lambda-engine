#include "selection.h"
#include "internal.h"
#include "debug.h"
#include "billboard.h"

void selection_render(mat4 projection)
{
	mat4 a, scaled_projection;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;

	mat4_identity(a);
	mat4_scale(a, a, (float)GameEngine.Graphics.Width, (float)GameEngine.Graphics.Height, 1.0f);
	mat4_mul(scaled_projection, a, projection);

	{
		/* Selection ID */
		u16 select_id[4];

		/* 3D Renderer */
		GL_CHECK(glUseProgram(renderer->ShaderSelection));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->SelectionFBO));
		GL_CHECK(glViewport(0, 0, 1, 1));
		GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		scene_render_selection(scaled_projection);

		/* Billboard Renderer */
		billboard_selection_render(scaled_projection);

		GL_CHECK(glReadPixels(0, 0, 1, 1, GL_BGRA, GL_UNSIGNED_SHORT, &select_id));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		renderer->SelectionID = select_id[2];
		renderer->SelectionX = select_id[1];
		renderer->SelectionY = select_id[0];
	}

	{
		/* Selection Position */
		float select_id[4];

		/* 3D Renderer */
		GL_CHECK(glBindVertexArray(renderer->Meshes->VAO));
		GL_CHECK(glUseProgram(renderer->ShaderSelectionPosition));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->SelectionPositionFBO));
		GL_CHECK(glViewport(0, 0, 1, 1));
		GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		scene_render_selection_position(scaled_projection);

		/* Billboard Renderer */
		/* billboard_selection_position_render(scaled_projection); */

		GL_CHECK(glReadPixels(0, 0, 1, 1, GL_BGRA, GL_FLOAT, &select_id));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		renderer->SelectionPosition[X] = select_id[2];
		renderer->SelectionPosition[Y] = select_id[1];
		renderer->SelectionPosition[Z] = select_id[0];
	}
}

