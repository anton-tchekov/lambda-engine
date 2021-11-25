#include "renderer3d.h"
#include "opengl.h"
#include "internal.h"
#include "debug.h"
#include "log.h"
#include "shader.h"
#include "shaders.h"
#include "strings.h"
#include "uniform.h"
#include "selection.h"
#include "billboard.h"
#include "skybox.h"
#include "wireframe.h"
#include "csm.h"
#include "blur.h"
#include <stdlib.h>

#define MATERIAL_DEFAULT_SHININESS  256.0f

static int _lighting_init(void);
static void _lighting_destroy(void);

static int _selection_init(void);
static void _selection_destroy(void);

static int _lighting_init(void)
{
	GLuint shader;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	log_debug("Loading rendering shaders");
	if(!(shader = shader_load_string_vf(lighting_vs, lighting_fs)))
	{
		return 1;
	}

	/* Lighting */
	renderer->ShaderLighting = shader;
	GL_CHECK(glUseProgram(shader));
	renderer->UniformLighting_Perspective = uniform_get(shader, s_uPerspective);
	renderer->UniformLighting_PositionMatrix = uniform_get(shader, s_uPositionMatrix);
	renderer->UniformLighting_Shadow = uniform_get(shader, s_uShadow);

	renderer->UniformLighting_ViewPosition = uniform_get(shader, s_uViewPosition);

	renderer->UniformLighting_LayerDiffuse = uniform_get(shader, s_uLayerDiffuse);
	renderer->UniformLighting_LayerSpecular = uniform_get(shader, s_uLayerSpecular);
	renderer->UniformLighting_LayerEmissive = uniform_get(shader, s_uLayerEmissive);
	renderer->UniformLighting_LayerNormal = uniform_get(shader, s_uLayerNormal);

	renderer->UniformLighting_Shininess = uniform_get(shader, s_uShininess);

	renderer->UniformLighting_SamplerDiffuse = uniform_get(shader, s_uSamplerDiffuse);
	renderer->UniformLighting_SamplerSpecular = uniform_get(shader, s_uSamplerSpecular);
	renderer->UniformLighting_SamplerEmissive = uniform_get(shader, s_uSamplerEmissive);
	renderer->UniformLighting_SamplerNormal = uniform_get(shader, s_uSamplerNormal);

	renderer->UniformLighting_ColorDiffuse = uniform_get(shader, s_uColorDiffuse);
	renderer->UniformLighting_ColorSpecular = uniform_get(shader, s_uColorSpecular);
	renderer->UniformLighting_ColorEmissive = uniform_get(shader, s_uColorEmissive);

	renderer->UniformLighting_PL_Position = uniform_get(shader, s_uPL_Position);
	renderer->UniformLighting_PL_Color = uniform_get(shader, s_uPL_Color);
	renderer->UniformLighting_PL_Constant = uniform_get(shader, s_uPL_Constant);
	renderer->UniformLighting_PL_Linear = uniform_get(shader, s_uPL_Linear);
	renderer->UniformLighting_PL_Quadratic = uniform_get(shader, s_uPL_Quadratic);

	renderer->UniformLighting_Resolution = uniform_get(shader, s_uResolution);

	renderer->UniformLighting_Dark = uniform_get(shader, s_uDark);

	/* Shadow Mapping */
	renderer->UniformLighting_DL_Used = uniform_get(shader, s_uDL_Used);
	renderer->UniformLighting_DL_Position = uniform_get(shader, s_uDL_Position);
	renderer->UniformLighting_DL_Color = uniform_get(shader, s_uDL_Color);
	renderer->UniformLighting_SamplerShadow = uniform_get(shader, s_uSamplerShadow);

	GL_CHECK(glUniform1i(renderer->UniformLighting_SamplerShadow, TU_DRAW));

	renderer3d_dsl(0.3f, 0.3f, 0.7f);
	return 0;
}

static void _lighting_destroy(void)
{
	log_debug("Destroying 3D rendering shaders");
	GL_CHECK(glDeleteProgram(GameEngine.Graphics.Renderer3D.ShaderLighting));
}

static int _shadow_bw_init(void)
{
	GLuint shader;
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	log_debug("Loading ShadowBW shaders");
	if(!(shader = shader_load_string_vf(shadow_bw_vs, shadow_bw_fs)))
	{
		return 1;
	}

	renderer->ShaderShadowBW = shader;
	GL_CHECK(glUseProgram(shader));
	renderer->UniformShadowBW_Perspective = uniform_get(shader, s_uPerspective);
	renderer->UniformShadowBW_PositionMatrix = uniform_get(shader, s_uPositionMatrix);

	/* Shadow Mapping */
	renderer->UniformShadowBW_LightMatrix = uniform_get(shader, s_uLightMatrix);
	renderer->UniformShadowBW_DL_Position = uniform_get(shader, s_uDL_Position);
	renderer->UniformShadowBW_SamplerShadow = uniform_get(shader, s_uSamplerShadow);
	GL_CHECK(glUniform1i(renderer->UniformShadowBW_SamplerShadow, TU_SHADOW));

	renderer3d_dimensions(GameEngine.Graphics.Width, GameEngine.Graphics.Height);
	return 0;
}

static void _shadow_bw_destroy(void)
{
	log_debug("Destroying shadow BW shaders");
	GL_CHECK(glDeleteProgram(GameEngine.Graphics.Renderer3D.ShaderShadowBW));
}

static int _selection_init(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GLuint shader;
	log_debug("Loading selection shaders");
	if(!(shader = shader_load_string_vf(selection_vs, selection_fs)))
	{
		return 1;
	}

	renderer->ShaderSelection = shader;
	GL_CHECK(glUseProgram(shader));
	renderer->UniformSelection_Perspective = uniform_get(shader, s_uPerspective);
	renderer->UniformSelection_PositionMatrix = uniform_get(shader, s_uPositionMatrix);
	renderer->UniformSelection_SelectionID = uniform_get(shader, s_uSelectionID);

	if(!(shader = shader_load_string_vf(selection_position_vs, selection_position_fs)))
	{
		return 1;
	}

	renderer->ShaderSelectionPosition = shader;
	GL_CHECK(glUseProgram(shader));
	renderer->UniformSelectionPos_Perspective = uniform_get(shader, s_uPerspective);
	renderer->UniformSelectionPos_PositionMatrix = uniform_get(shader, s_uPositionMatrix);

	/* Selection */
	GL_CHECK(glGenFramebuffers(1, &renderer->SelectionFBO));
	GL_CHECK(glGenRenderbuffers(2, renderer->SelectionRBO));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->SelectionFBO));

	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderer->SelectionRBO[0]));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16, 1, 1));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderer->SelectionRBO[0]));

	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderer->SelectionRBO[1]));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1, 1));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderer->SelectionRBO[1]));

	/* Selection Position */
	GL_CHECK(glGenFramebuffers(1, &renderer->SelectionPositionFBO));
	GL_CHECK(glGenRenderbuffers(2, renderer->SelectionPositionRBO));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->SelectionPositionFBO));

	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderer->SelectionPositionRBO[0]));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, 1, 1));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderer->SelectionPositionRBO[0]));

	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderer->SelectionPositionRBO[1]));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1, 1));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderer->SelectionPositionRBO[1]));

	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return 0;
}

static void _selection_destroy(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	log_debug("Destroying selection rendering shaders");
	GL_CHECK(glDeleteProgram(renderer->ShaderSelection));
	GL_CHECK(glDeleteRenderbuffers(2, renderer->SelectionRBO));
	GL_CHECK(glDeleteFramebuffers(1, &renderer->SelectionFBO));
}

static int _shadow_init(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GLuint shader;

	if(!(renderer->ShadowVertices = malloc(SHADOW_VERTICES_BYTES)))
	{
		return 1;
	}

	if(!(renderer->ShadowIndices = malloc(SHADOW_INDICES_BYTES)))
	{
		free(renderer->ShadowVertices);
		return 1;
	}

	log_debug("Loading shadow shaders");
	if(!(shader = shader_load_string_vf(shadow_vs, shadow_fs)))
	{
		free(renderer->ShadowVertices);
		free(renderer->ShadowIndices);
		return 1;
	}

	renderer->ShaderShadow = shader;
	renderer->UniformShadowPerspective = uniform_get(shader, s_uPerspective);
	GL_CHECK(glGenVertexArrays(1, &renderer->ShadowVAO));
	GL_CHECK(glGenBuffers(2, renderer->ShadowVBO));
	GL_CHECK(glBindVertexArray(renderer->ShadowVAO));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, renderer->ShadowVBO[0]));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, SHADOW_VERTICES_BYTES, NULL, GL_STREAM_DRAW));
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ShadowVBO[1]));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, SHADOW_INDICES_BYTES, NULL, GL_STREAM_DRAW));
	return 0;
}

static void _shadow_destroy(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	log_debug("Destroying shadow rendering shaders");
	GL_CHECK(glDeleteProgram(renderer->ShaderShadow));
}

static int _shadow_map_init(void)
{
	unsigned int csm_texture, csm_framebuffer;
	const float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GLuint shader;
	log_debug("Loading shadow map shaders");
	if(!(shader = shader_load_string_vf(shadow_map_vs, shadow_map_fs)))
	{
		return 1;
	}

	renderer->ShaderShadowMap = shader;
	GL_CHECK(glUseProgram(shader));
	renderer->UniformShadowMap_Perspective = uniform_get(shader, s_uPerspective);
	renderer->UniformShadowMap_PositionMatrix = uniform_get(shader, s_uPositionMatrix);

	/* Shadow Map */
	GL_CHECK(glGenFramebuffers(1, &csm_framebuffer));
	GL_CHECK(glGenTextures(1, &csm_texture));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, csm_texture));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, CSM_SIZE, CSM_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, csm_framebuffer));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, csm_texture, 0));
	GL_CHECK(glDrawBuffer(GL_NONE));
	GL_CHECK(glReadBuffer(GL_NONE));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	renderer->ShadowFBO = csm_framebuffer;
	renderer->ShadowTexture = csm_texture;

	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_SHADOW));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, csm_texture));

	return 0;
}

static void _shadow_map_destroy(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	log_debug("Destroying shadow map rendering shaders");
	GL_CHECK(glDeleteProgram(renderer->ShaderShadowMap));
}

int renderer3d_init(void)
{
	log_debug("Initializing 3D renderer");
	if(_lighting_init())
	{
		goto fail_lighting;
	}

	if(_selection_init())
	{
		goto fail_selection;
	}

	if(_shadow_init())
	{
		goto fail_shadow;
	}

	if(_shadow_map_init())
	{
		goto fail_shadow_map;
	}

	if(_shadow_bw_init())
	{
		goto fail_shadow_bw;
	}

	return 0;

fail_shadow_bw:
	_shadow_bw_destroy();

fail_shadow_map:
	_shadow_destroy();

fail_shadow:
	_selection_destroy();

fail_selection:
	_lighting_destroy();

fail_lighting:
	return 1;
}

void renderer3d_dsl(float dark, float shadow, float light)
{
	GameEngine.Graphics.Renderer3D.Dark = dark;
	GameEngine.Graphics.Renderer3D.Shadow = shadow;
	GameEngine.Graphics.Renderer3D.Light = light;
}

void renderer3d_bind_mesh_collection(MeshCollection *mc0)
{
	u32 i;
	GameEngine.Graphics.Renderer3D.Meshes = mc0;

	/* Texture arrays of size n */
	for(i = 0; i < NUM_TEXTURE_LAYERS; ++i)
	{
		if(mc0->Textures[i])
		{
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_ARRAY + i));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, mc0->Textures[i]));
		}
	}
}

void renderer3d_bind_shadow_collection(ShadowCollection *sc0)
{
	GameEngine.Graphics.Renderer3D.Shadows = sc0;
}

void renderer3d_destroy(void)
{
	log_debug("Destroying 3D renderer");
	_selection_destroy();
	_shadow_destroy();
	_shadow_map_destroy();
	_shadow_bw_destroy();
	_lighting_destroy();
}

void renderer3d_bind_scene(Scene *scene0)
{
	GameEngine.Graphics.Renderer3D.Scene = scene0;
}

void renderer3d_bind_camera(Camera *camera)
{
	GameEngine.Graphics.Renderer3D.Camera = camera;
}

void renderer3d_bind_world(World *world0)
{
	GameEngine.Graphics.Renderer3D.World = world0;
}

void shadow_bw_render(mat4 perspective)
{
	/* RENDER */
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->DrawFBO));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	scene_render_shadow_bw(perspective);

	/* BLUR */
	gblur5();

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

int renderer3d_render(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;
	Lighting *l;
	mat4 lmat;

	if(!renderer->Camera || !renderer->Meshes || !renderer->Scene)
	{
		return 1;
	}

	l = renderer->Scene->Lighting;

	GL_CHECK(glEnable(GL_CULL_FACE));
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthFunc(GL_LESS));
	GL_CHECK(glCullFace(GL_BACK));
	GL_CHECK(glBindVertexArray(renderer->Meshes->VAO));

	scene_prepare();

	/* Render Shadow Maps */
	if(GameEngine.Graphics.RenderShadowMap)
	{
		mat4 lview, lproj;
		vec3 center = { 0.0f, 0.0f, 0.0f }, up = { 0.0f, 1.0f, 0.0f };
		float near_plane = 1.0f, far_plane = 50.5f;
		mat4_ortho(lproj, -10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		mat4_look_at(lview, renderer->Scene->Lighting->DirLight.Position, center, up);
		mat4_mul(lmat, lproj, lview);
		csm_render(lmat);
	}

	/* Render Selection */
	if(GameEngine.Graphics.RenderSelection)
	{
		selection_render(renderer->Camera->Perspective);
	}

	{
		int j, m = 1 + GameEngine.Graphics.VR_Mode;
		int w = GameEngine.Graphics.Width / m;
		mat4 *pers = m == 1 ? &renderer->Camera->Perspective : renderer->Camera->PerspectiveEye;
		for(j = 0; j < m; ++j)
		{
			GL_CHECK(glViewport(j * w, 0, w, GameEngine.Graphics.Height));

			/* Render Shadow BW */
			if(GameEngine.Graphics.RenderShadowMap)
			{
				GL_CHECK(glUseProgram(renderer->ShaderShadowBW));
				GL_CHECK(glUniform3f(
					renderer->UniformShadowBW_DL_Position,
					l->DirLight.Position[0],
					l->DirLight.Position[1],
					l->DirLight.Position[2]));

				GL_CHECK(glUniformMatrix4fv(renderer->UniformShadowBW_LightMatrix, 1, GL_FALSE, &lmat[0][0]));
				shadow_bw_render(pers[j]);
				GL_CHECK(glBindVertexArray(renderer->Meshes->VAO));
				GL_CHECK(glViewport(j * w, 0, w, GameEngine.Graphics.Height));
			}

			/* Render Lighting */
			GL_CHECK(glUseProgram(renderer->ShaderLighting));

			if(j == 0)
			{
				GL_CHECK(glUniform1i(renderer->UniformLighting_DL_Used, GameEngine.Graphics.RenderShadowMap));

				/* Directional Lights */
				GL_CHECK(glUniform3f(
					renderer->UniformLighting_DL_Position,
					l->DirLight.Position[0],
					l->DirLight.Position[1],
					l->DirLight.Position[2]));

				GL_CHECK(glUniform3f(
					renderer->UniformLighting_DL_Color,
					l->DirLight.Color[0],
					l->DirLight.Color[1],
					l->DirLight.Color[2]));

				/* Point Lights */
				GL_CHECK(glUniform3f(renderer->UniformLighting_ViewPosition,
						renderer->Camera->Position[X], renderer->Camera->Position[Y], renderer->Camera->Position[Z]));

				GL_CHECK(glUniform3fv(
					renderer->UniformLighting_PL_Position,
					NUM_POINT_LIGHTS, &l->PointLights.Position[0][0]));

				GL_CHECK(glUniform3fv(
					renderer->UniformLighting_PL_Color,
					NUM_POINT_LIGHTS, &l->PointLights.Color[0][0]));

				GL_CHECK(glUniform1fv(
					renderer->UniformLighting_PL_Constant,
					NUM_POINT_LIGHTS, l->PointLights.Constant));

				GL_CHECK(glUniform1fv(
					renderer->UniformLighting_PL_Linear,
					NUM_POINT_LIGHTS, l->PointLights.Linear));

				GL_CHECK(glUniform1fv(
					renderer->UniformLighting_PL_Quadratic,
					NUM_POINT_LIGHTS, l->PointLights.Quadratic));

				GL_CHECK(glUniform2f(renderer->UniformLighting_Resolution, (float)w, (float)GameEngine.Graphics.Height));

				GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
				GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
			}

			if(GameEngine.Graphics.AA_Method == AA_MSAA)
			{
				GL_CHECK(glEnable(GL_MULTISAMPLE));
			}

			GL_CHECK(glUniformMatrix4fv(renderer->UniformLighting_Perspective, 1, GL_FALSE, &pers[j][0][0]));
			GL_CHECK(glUniform1f(renderer->UniformLighting_Dark, renderer->Dark));

			{
				/* Render in shadow (VOLUMES) */
				float s = GameEngine.Graphics.RenderShadowVolume ? renderer->Shadow : renderer->Light;
				GL_CHECK(glDisable(GL_STENCIL_TEST));
				GL_CHECK(glStencilMask(0xFF));
				GL_CHECK(glDepthMask(GL_TRUE));
				GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
				GL_CHECK(glUniform1f(renderer->UniformLighting_Shadow, s));
				GL_CHECK(glBindVertexArray(renderer->Meshes->VAO));
				scene_render_lighting();
			}

			if(GameEngine.Graphics.RenderShadowVolume)
			{
				/* Render Billboards */
				billboard_render(pers[j], renderer->Shadow);

				/* Render shadow volumes */
				GL_CHECK(glDisable(GL_CULL_FACE));
				GL_CHECK(glEnable(GL_STENCIL_TEST));
				GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
				GL_CHECK(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
				GL_CHECK(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));
				GL_CHECK(glDepthMask(GL_FALSE));
				GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

				GL_CHECK(glUseProgram(renderer->ShaderShadow));
				scene_render_shadow(pers[j]);

				/* Render with lighting */
				GL_CHECK(glUseProgram(renderer->ShaderLighting));
				GL_CHECK(glEnable(GL_CULL_FACE));
				GL_CHECK(glStencilFunc(GL_EQUAL, 0, 0xFF));
				GL_CHECK(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP));
				GL_CHECK(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP));
				GL_CHECK(glDepthMask(GL_TRUE));
				GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
				GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
				GL_CHECK(glUniform1f(renderer->UniformLighting_Shadow, renderer->Light));
				GL_CHECK(glBindVertexArray(renderer->Meshes->VAO));
				scene_render_lighting();
			}

			GL_CHECK(glDepthMask(GL_FALSE));

			/* Render Skybox */
			skybox_render(renderer->Camera->PerspectiveSkybox);

			/* Render Billboards */
			billboard_render(pers[j], renderer->Light);
			GL_CHECK(glDepthMask(GL_TRUE));

			GL_CHECK(glDisable(GL_STENCIL_TEST));
			if(GameEngine.Graphics.RenderWireframe)
			{
				wireframe_use(pers[j]);
				for(u32 i = 0; i < renderer->World->NumBoxes; ++i)
				{
					wireframe_aabb(renderer->World->Boxes + i);
				}

				wireframe_render(COLOR.GREEN);
			}

			if(GameEngine.Graphics.AA_Method == AA_MSAA)
			{
				GL_CHECK(glDisable(GL_MULTISAMPLE));
			}
		}
	}


	/* DEBUG */
	/*GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->DrawFBO));
	GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	GL_CHECK(glBlitFramebuffer(0, 0, GameEngine.Graphics.Width, GameEngine.Graphics.Height,
			0, 0, GameEngine.Graphics.Width, GameEngine.Graphics.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));*/

	return 0;
}

void renderer3d_fbo_destroy(void)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;

	GL_CHECK(glDeleteFramebuffers(1, &renderer->DrawFBO));
	GL_CHECK(glDeleteRenderbuffers(1, &renderer->DrawRBO));
	GL_CHECK(glDeleteTextures(1, &renderer->DrawTex));

	GL_CHECK(glDeleteFramebuffers(1, &renderer->BlurFBO));
	GL_CHECK(glDeleteTextures(1, &renderer->BlurTex));
}

void renderer3d_dimensions(int w, int h)
{
	Renderer3D *renderer = &GameEngine.Graphics.Renderer3D;

	/* --- Draw Buffer --- */
	GL_CHECK(glGenFramebuffers(1, &renderer->DrawFBO));
	GL_CHECK(glGenRenderbuffers(1, &renderer->DrawRBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->DrawFBO));

	/* Color Attachment */
	GL_CHECK(glGenTextures(1, &renderer->DrawTex));
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_DRAW));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderer->DrawTex));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->DrawTex, 0));

	/* Depth Attachment */
	GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderer->DrawRBO));
	GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h));
	GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderer->DrawRBO));

	/* --- Blur Buffer --- */
	GL_CHECK(glGenFramebuffers(1, &renderer->BlurFBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, renderer->BlurFBO));

	/* Color Attachment */
	GL_CHECK(glGenTextures(1, &renderer->BlurTex));
	GL_CHECK(glActiveTexture(GL_TEXTURE0 + TU_BLUR));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, renderer->BlurTex));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->BlurTex, 0));
}

