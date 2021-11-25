#include "support.h"
#include "opengl.h"
#include "internal.h"
#include "debug.h"

static const GLenum _gl_enums[SUPPORT_NUM_VALUES] =
{
	GL_MAX_DRAW_BUFFERS,
	GL_MAX_RENDERBUFFER_SIZE,
	GL_MAX_TEXTURE_IMAGE_UNITS,
	GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
	GL_MAX_TEXTURE_SIZE,
	GL_MAX_CUBE_MAP_TEXTURE_SIZE,
	GL_MAX_ARRAY_TEXTURE_LAYERS,
	GL_MAX_COMBINED_UNIFORM_BLOCKS,
	GL_MAX_VERTEX_UNIFORM_COMPONENTS,
	GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
	GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
	GL_MAX_ELEMENTS_INDICES,
	GL_MAX_ELEMENTS_VERTICES,
	GL_MAX_INTEGER_SAMPLES
};

void support_get(void)
{
	u32 i;
	for(i = 0; i < SUPPORT_NUM_VALUES; ++i)
	{
		GL_CHECK(glGetIntegerv(_gl_enums[i], &GameEngine.Graphics.Support.Values[i]));
	}
}

#ifdef DEBUG
void support_log(void)
{
	log_debug("Printing relevant GPU/Driver supported features and limitations: (MINIMUM/SUPPORTED)");

	log_debug("Maximum renderbuffer size: %d",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_RENDERBUFFER_SIZE]);

	log_debug("Maximum texture size: (1024x1024/%dx%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_TEXTURE_SIZE],
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_TEXTURE_SIZE]);

	log_debug("Maximum cubemap texture size: %dx%d",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_CUBE_MAP_TEXTURE_SIZE],
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_CUBE_MAP_TEXTURE_SIZE]);

	log_debug("Maximum texture array size: (256/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_ARRAY_TEXTURE_LAYERS]);

	log_debug("Maximum multisample samples per pixel: %d",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_INTEGER_SAMPLES]);

	log_debug("Recommended maximum index buffer size: %d",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_ELEMENTS_INDICES]);

	log_debug("Recommended maximum vertex buffer size: %d",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_ELEMENTS_VERTICES]);

	log_debug("Maximum number of uniforms per shader program: (70/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_COMBINED_UNIFORM_BLOCKS]);

	log_debug("Maximum number of scalar uniforms in vertex shader: (1024/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_VERTEX_UNIFORM_COMPONENTS]);

	log_debug("Maximum number of scalar uniforms in geometry shader: (1024/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_GEOMETRY_UNIFORM_COMPONENTS]);

	log_debug("Maximum number of scalar uniforms in fragment shader: (1024/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_FRAGMENT_UNIFORM_COMPONENTS]);

	log_debug("Maximum number of texture units bound to a shader program: (16/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_TEXTURE_IMAGE_UNITS]);

	log_debug("Maximum number of render targets bound to a shader program: (8/%d)",
			GameEngine.Graphics.Support.Values[SUPPORT_MAX_DRAW_BUFFERS]);
}
#endif

