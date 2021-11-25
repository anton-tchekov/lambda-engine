#ifndef __INTERNAL__
#define __INTERNAL__

#include "types.h"
#include "vector.h"
#include "color.h"
#include "texture.h"
#include "font.h"
#include "openal.h"
#include "opengl.h"
#include "aabb.h"
#include "scene.h"
#include "camera.h"
#include "mesh.h"
#include "shadow.h"
#include "defines.h"

enum TEXTURE_UNITS
{
	TU_BIND,
	TU_SPRITE,
	TU_BILLBOARD,
	TU_SKYBOX,
	TU_SHADOW,
	TU_DRAW,
	TU_BLUR,
	TU_ARRAY,
	TU_ARRAY_32,
	TU_ARRAY_64,
	TU_ARRAY_128,
	TU_ARRAY_256,
	TU_ARRAY_512,
	TU_ARRAY_1024
};

extern const GLsizei _layer_sizes[NUM_TEXTURE_LAYERS];

/* GPU Support */
enum SUPPORT_INDICES
{
	SUPPORT_MAX_DRAW_BUFFERS,
	SUPPORT_MAX_RENDERBUFFER_SIZE,
	SUPPORT_MAX_TEXTURE_IMAGE_UNITS,
	SUPPORT_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
	SUPPORT_MAX_TEXTURE_SIZE,
	SUPPORT_MAX_CUBE_MAP_TEXTURE_SIZE,
	SUPPORT_MAX_ARRAY_TEXTURE_LAYERS,
	SUPPORT_MAX_COMBINED_UNIFORM_BLOCKS,
	SUPPORT_MAX_VERTEX_UNIFORM_COMPONENTS,
	SUPPORT_MAX_GEOMETRY_UNIFORM_COMPONENTS,
	SUPPORT_MAX_FRAGMENT_UNIFORM_COMPONENTS,
	SUPPORT_MAX_ELEMENTS_INDICES,
	SUPPORT_MAX_ELEMENTS_VERTICES,
	SUPPORT_MAX_INTEGER_SAMPLES,
	SUPPORT_NUM_VALUES
};

typedef struct SUPPORT
{
	GLint Values[SUPPORT_NUM_VALUES];
} Support;

typedef struct SKYBOX
{
	GLuint Shader, VBO[2], VAO;
	GLint Uniform_Perspective;
} Skybox;

typedef struct BILLBOARD
{
	u8 *Vertices, *Indices;
	GLsizei MaxBillboards, NumBillboards, BillboardIndex;
	GLuint Shader, ShaderSelection, VBO[2], VAO, FBO, Texture;
	GLint Uniform_Perspective,
			Uniform_PL_Position,
			Uniform_PL_Color,
			Uniform_PL_Constant,
			Uniform_PL_Linear,
			Uniform_PL_Quadratic,
			Uniform_Shadow,
			Uniform_Dark,
			Uniform_SelectionPerspective;
	mat4 Projection;
} Billboard;

typedef struct RENDERER2D
{
	u32 FontDirty : 1;
	u32 TextureDirty : 1;
	mat4 ProjectionWindow;
	GLuint Shader;
	GLint UniformProjection, UniformTextureSize,
			UniformFontProjection, UniformFontTextureSize;
	Texture *BoundTexture;
	FontCollection *BoundFC;
	u8 *Vertices;
	u16 *Indices;
	GLsizei NumSprites,
			BillboardNumClears, BillboardNumSprites,
			NumTotalSprites, MaxTotalSprites;
	GLuint VBO[2], VAO;
	u32 FontID, SizeID, LineHeight, MaxWidth;
	ColorRGBA8 Color;
} Renderer2D;

enum AA_METHOD
{
	AA_NOAA,
	AA_MSAA
};

typedef struct WORLD
{
	float Gravity;
	AABB *Boxes;
	u32 NumBoxes;
} World;

typedef struct RENDERER3D
{
	GLuint ShaderLighting,
			ShaderShadowBW,
			ShaderSelection,
			ShaderSelectionPosition,
			SelectionFBO,
			SelectionRBO[2],
			SelectionPositionFBO,
			SelectionPositionRBO[2],
			ShadowVBO[2], ShadowVAO,
			ShaderShadow,
			ShaderShadowMap,
			ShadowFBO,
			ShadowTexture,
			DrawFBO,
			DrawRBO,
			DrawTex,
			BlurFBO,
			BlurTex;

	GLint UniformLighting_LayerDiffuse,
			UniformLighting_LayerSpecular,
			UniformLighting_LayerEmissive,
			UniformLighting_LayerNormal,

			UniformLighting_SamplerDiffuse,
			UniformLighting_SamplerSpecular,
			UniformLighting_SamplerEmissive,
			UniformLighting_SamplerNormal,

			UniformLighting_ColorDiffuse,
			UniformLighting_ColorSpecular,
			UniformLighting_ColorEmissive,

			UniformLighting_PL_Position,
			UniformLighting_PL_Color,
			UniformLighting_PL_Constant,
			UniformLighting_PL_Linear,
			UniformLighting_PL_Quadratic,
			UniformLighting_Perspective,
			UniformLighting_PositionMatrix,
			UniformLighting_Shadow,
			UniformLighting_ViewPosition,
			UniformLighting_Shininess,
			UniformLighting_Dark,
			UniformLighting_Resolution,

			UniformLighting_DL_Used,
			UniformLighting_DL_Position,
			UniformLighting_DL_Color,
			UniformLighting_SamplerShadow,

			UniformShadowBW_Perspective,
			UniformShadowBW_PositionMatrix,

			UniformShadowBW_LayerDiffuse,
			UniformShadowBW_SamplerDiffuse,

			UniformShadowBW_LightMatrix,
			UniformShadowBW_DL_Position,
			UniformShadowBW_SamplerShadow,

			UniformDepth_Perspective,
			UniformDepth_PositionMatrix,

			UniformShadowPerspective,

			UniformSelection_Perspective,
			UniformSelection_PositionMatrix,
			UniformSelection_SelectionID,

			UniformSelectionPos_Perspective,
			UniformSelectionPos_PositionMatrix,

			UniformShadowMap_Perspective,
			UniformShadowMap_PositionMatrix;

	World *World;

	float Dark, Shadow, Light;
	u32 SelectionID, SelectionX, SelectionY;
	vec3 SelectionPosition;
	ShadowCollection *Shadows;
	MeshCollection *Meshes;
	Scene *Scene;
	Camera *Camera;

	vec3 *ShadowVertices;
	u32 *ShadowIndices, ShadowNumVertices, ShadowNumIndices;
} Renderer3D;

typedef struct GRAPHICS
{
	u32 Fullscreen : 1;
	u32 VSync : 1;
	u32 AA_Method : 1;
	u32 EventMode : 1;
	u32 Render3D : 1;
	u32 RenderSelection : 1;
	u32 VR_Mode : 1;
	u32 Screenshot : 1;
	u32 RenderShadowVolume : 1;
	u32 RenderShadowMap : 1;
	u32 RenderWireframe : 1;
	u32 VBO_PS, VAO_PS, TEX_PS, FBO_PS, DRW_PS, VTX_PS,
		VBO_CT, VAO_CT, TEX_CT, FBO_CT, DRW_CT, VTX_CT, FPS, Frames;
	double FrameTime, FPS_Time;
	const char *Title;
	int MinWidth, MinHeight, WindowX, WindowY, WindowWidth, WindowHeight;
	GLsizei Width, Height;
	float AspectRatio;
	GLFWwindow *Window;
	Support Support;
	Renderer2D Renderer2D;
	Renderer3D Renderer3D;
	Billboard Billboard;
	Skybox Skybox;
} Graphics;

typedef struct AUDIO
{
	ALCdevice *Device;
	ALCcontext *Context;
} Audio;

typedef struct ENGINE
{
	Graphics Graphics;
	Audio Audio;
	double CurrentTime, LastTime, DeltaTime;
} Engine;

extern Engine GameEngine;

#endif

