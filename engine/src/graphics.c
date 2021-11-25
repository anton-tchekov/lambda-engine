#include "graphics.h"
#include "opengl.h"
#include "log.h"
#include "internal.h"
#include "support.h"
#include "skybox.h"
#include "billboard.h"
#include "renderer2d.h"
#include "renderer3d.h"
#include "wireframe.h"
#include "blur.h"

#ifdef DEBUG
static void callback_error(int error, const char *description);
#endif

#define MAX_BILLBOARDS    (10 * 1024)
#define MAX_TOTAL_SPRITES (10 * 1024)

static void callback_resize(GLFWwindow *window, int width, int height);
static void callback_enter(GLFWwindow *window, int entered);

static void callback_key(GLFWwindow *window, int key, int scancode, int action, int mods);
static void callback_char(GLFWwindow *window, unsigned int codepoint);
static void callback_mouse(GLFWwindow *window, double pos_x, double pos_y);
static void callback_button(GLFWwindow *window, int button, int action, int mods);
static void callback_scroll(GLFWwindow *window, double offset_x, double offset_y);

void event_resize(int width, int height);
void event_key(int key, int scancode, int action, int mods);
void event_char(unsigned int codepoint);
void event_mouse(double pos_x, double pos_y);
void event_button(int button, int action, int mods);
void event_scroll(double offset_x, double offset_y);
void event_enter(void);
void event_leave(void);

static void _update_size(void);

static int graphics_glfw(const char *title, int min_w, int min_h)
{
	log_debug("Initializing graphics");
	GameEngine.Graphics.AA_Method = AA_NOAA;
	GameEngine.Graphics.VSync = 1;
	GameEngine.Graphics.Fullscreen = 0;
	GameEngine.Graphics.Render3D = 1;
	GameEngine.Graphics.Screenshot = 0;
	GameEngine.Graphics.RenderSelection = 1;

	/* Initialize GLFW */
	log_debug("Initializing GLFW");
	log_debug("Setting GLFW error callback");

#ifdef DEBUG
	glfwSetErrorCallback(callback_error);
#endif
	if(!glfwInit())
	{
		return 1;
	}

	/* Check if raw mouse motion is supported */
	log_debug("Checking for raw mouse motion");
	if(!glfwRawMouseMotionSupported())
	{
		log_error("Raw mouse motion is not supported");
		glfwTerminate();
		return 1;
	}

	log_debug("Creating window");

	/* OpenGL Version 3.3 Core Profile */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Double buffering */
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Hide Window */
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	/* Maximized Window */
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	/* Forward compatibility context */
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef DEBUG
	/* Debug context */
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GameEngine.Graphics.MinWidth = min_w;
	GameEngine.Graphics.MinHeight = min_h;
	GameEngine.Graphics.Title = title;

	/* TEMP */
	//GameEngine.Graphics.RenderShadowVolume = 1;
	//GameEngine.Graphics.RenderShadowMap = 1;
	/* TEMP */


	/* Create Window */
	if(!(GameEngine.Graphics.Window = glfwCreateWindow(
			GameEngine.Graphics.MinWidth,
			GameEngine.Graphics.MinHeight,
			GameEngine.Graphics.Title, NULL, NULL)))
	{
		log_error("Creating window failed");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(GameEngine.Graphics.Window);

	/* Enable VSync */
	glfwSwapInterval(1);

	/* Set minimum GameEngine.Graphics.Window size */
	glfwSetWindowSizeLimits(GameEngine.Graphics.Window,
			GameEngine.Graphics.MinWidth,
			GameEngine.Graphics.MinHeight,
			GLFW_DONT_CARE, GLFW_DONT_CARE);

	log_debug("Loading OpenGL functions");

	/* Load OpenGL functions */
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		log_error("Loading OpenGL functions failed");
		glfwDestroyWindow(GameEngine.Graphics.Window);
		glfwTerminate();
		return 1;
	}

	/* Get GameEngine.Graphics.Window size */
	glfwGetFramebufferSize(
			GameEngine.Graphics.Window,
			&GameEngine.Graphics.Width,
			&GameEngine.Graphics.Height);

	/* Set events */
	glfwSetFramebufferSizeCallback(GameEngine.Graphics.Window, callback_resize);
	glfwSetKeyCallback(GameEngine.Graphics.Window, callback_key);
	glfwSetCharCallback(GameEngine.Graphics.Window, callback_char);
	glfwSetCursorPosCallback(GameEngine.Graphics.Window, callback_mouse);
	glfwSetMouseButtonCallback(GameEngine.Graphics.Window, callback_button);
	glfwSetScrollCallback(GameEngine.Graphics.Window, callback_scroll);
	glfwSetCursorEnterCallback(GameEngine.Graphics.Window, callback_enter);
	return 0;
}

int graphics_init(const char *title, int min_w, int min_h)
{
	if(graphics_glfw(title, min_w, min_h))
	{
		log_error("Initializing graphics context and window failed");
		return 1;
	}

	support_get();
#ifdef DEBUG
	support_log();
#endif

	/* 2D Renderer */
	if(renderer2d_init(MAX_TOTAL_SPRITES))
	{
		log_error("Initializing 2D renderer failed");
		goto fail_2d_renderer;
	}

	if(billboard_init(MAX_BILLBOARDS))
	{
		log_error("Initializing billboard failed");
		goto fail_billboard;
	}

	/* 3D Renderer */
	if(renderer3d_init())
	{
		log_error("Initializing 3D renderer failed");
		goto fail_3d_renderer;
	}

	/* Skybox */
	if(skybox_init())
	{
		log_error("Initializing skybox failed");
		goto fail_skybox;
	}

	/* Wireframe */
	if(wireframe_init())
	{
		log_error("Initializing wireframe failed");
		goto fail_wireframe;
	}

	if(gblur_init())
	{
		log_error("Initializing blur failed");
		goto fail_blur;
	}

	GameEngine.Graphics.AspectRatio = (float)GameEngine.Graphics.Width / (float)GameEngine.Graphics.Height;
	renderer2d_dimensions(GameEngine.Graphics.Renderer2D.ProjectionWindow, GameEngine.Graphics.Width, GameEngine.Graphics.Height);
	return 0;

fail_blur:
	wireframe_destroy();

fail_wireframe:
	skybox_destroy();

fail_skybox:
	renderer3d_destroy();

fail_3d_renderer:
	renderer2d_destroy();

fail_billboard:
	billboard_destroy();

fail_2d_renderer:
	glfwDestroyWindow(GameEngine.Graphics.Window);
	glfwTerminate();
	return 1;
}

int key_pressed(int key)
{
	return glfwGetKey(GameEngine.Graphics.Window, key) == GLFW_PRESS;
}

void graphics_destroy(void)
{
	log_debug("Destroying graphics");
	renderer2d_destroy();
	renderer3d_destroy();
	skybox_destroy();
	gblur_destroy();

	log_debug("Destroying window");
	glfwDestroyWindow(GameEngine.Graphics.Window);

	log_debug("Destroying GLFW");
	glfwTerminate();
}

/* Fullscreen / Windowed */
void graphics_fullscreen(void)
{
	GLFWmonitor *monitor;
	const GLFWvidmode *video_mode;

	/* Save window position and dimensions */
	glfwGetWindowPos(GameEngine.Graphics.Window, &GameEngine.Graphics.WindowX, &GameEngine.Graphics.WindowY);
	glfwGetWindowSize(GameEngine.Graphics.Window, &GameEngine.Graphics.WindowWidth, &GameEngine.Graphics.WindowHeight);

	/* Go into fullscreen mode */
	monitor = glfwGetPrimaryMonitor();
	video_mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(GameEngine.Graphics.Window, monitor, 0, 0, video_mode->width, video_mode->height, 0);
	GameEngine.Graphics.Width = video_mode->width;
	GameEngine.Graphics.Height = video_mode->height;
	GameEngine.Graphics.Fullscreen = 1;
}

void graphics_windowed(void)
{
	/* Restore window */
	glfwSetWindowMonitor(GameEngine.Graphics.Window, NULL,
			GameEngine.Graphics.WindowX, GameEngine.Graphics.WindowY,
			GameEngine.Graphics.WindowWidth, GameEngine.Graphics.WindowHeight, 0);
	GameEngine.Graphics.Fullscreen = 0;
}

void graphics_screenshot(void)
{
	GameEngine.Graphics.Screenshot = 1;
}

int graphics_get_mode(void)
{
	return GameEngine.Graphics.Fullscreen;
}

void graphics_mode(u32 n)
{
	if(n)
	{
		graphics_fullscreen();
	}
	else
	{
		graphics_windowed();
	}
}

void graphics_wireframe(int i)
{
	GameEngine.Graphics.RenderWireframe = i;
}

int graphics_get_wireframe(void)
{
	return GameEngine.Graphics.RenderWireframe;
}

void graphics_aa_method(int method)
{
	GameEngine.Graphics.AA_Method = method;
}

int graphics_get_aa_method(void)
{
	return GameEngine.Graphics.AA_Method;
}

void graphics_vr_mode(int vr)
{
	GameEngine.Graphics.VR_Mode = vr;
}

int graphics_get_vr_mode(void)
{
	return GameEngine.Graphics.VR_Mode;
}

int graphics_get_vsync(void)
{
	return GameEngine.Graphics.VSync;
}

void graphics_get_selpos_v3(vec3 pos)
{
	vec3_dup(pos, GameEngine.Graphics.Renderer3D.SelectionPosition);
}

void graphics_get_sel(u32 *x, u32 *y, u32 *z)
{
	*x = GameEngine.Graphics.Renderer3D.SelectionID;
	*y = GameEngine.Graphics.Renderer3D.SelectionX;
	*z = GameEngine.Graphics.Renderer3D.SelectionY;
}

void graphics_get_selpos(float *x, float *y, float *z)
{
	*x = GameEngine.Graphics.Renderer3D.SelectionPosition[X];
	*y = GameEngine.Graphics.Renderer3D.SelectionPosition[Y];
	*z = GameEngine.Graphics.Renderer3D.SelectionPosition[Z];
}

u32 graphics_vbo_binds(void)
{
	return GameEngine.Graphics.VBO_PS;
}

u32 graphics_vao_binds(void)
{
	return GameEngine.Graphics.VAO_PS;
}

u32 graphics_texture_binds(void)
{
	return GameEngine.Graphics.TEX_PS;
}

u32 graphics_fbo_binds(void)
{
	return GameEngine.Graphics.FBO_PS;
}

u32 graphics_draw_calls(void)
{
	return GameEngine.Graphics.DRW_PS;
}

u32 graphics_vertices(void)
{
	return GameEngine.Graphics.VTX_PS;
}

/* Event Mode */
void graphics_event_mode_enable(void)
{
	GameEngine.Graphics.EventMode = 1;
	glfwSwapInterval(0);
}

void graphics_event_mode_disable(void)
{
	GameEngine.Graphics.EventMode = 0;
	graphics_vsync(GameEngine.Graphics.VSync);
}

/* VSync */
void graphics_vsync_enable(void)
{
	glfwSwapInterval(1);
	GameEngine.Graphics.VSync = 1;
}

void graphics_vsync_disable(void)
{
	glfwSwapInterval(0);
	GameEngine.Graphics.VSync = 0;
}

void graphics_vsync(u32 n)
{
	if(n)
	{
		graphics_vsync_enable();
	}
	else
	{
		graphics_vsync_disable();
	}
}

/* Mouse Lock */
void graphics_mouse_lock(void)
{
	glfwSetInputMode(GameEngine.Graphics.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void graphics_mouse_unlock(void)
{
	glfwSetInputMode(GameEngine.Graphics.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

/* Callbacks */
void callback_enter(GLFWwindow *window, int entered)
{
	if(entered)
	{
		event_enter();
	}
	else
	{
		event_leave();
	}

	(void)window;
}

static void callback_resize(GLFWwindow *window, int width, int height)
{
	GameEngine.Graphics.Width = width;
	GameEngine.Graphics.Height = height;
	_update_size();
	event_resize(width, height);
	(void)window;
}

static void callback_key(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	event_key(key, scancode, action, mods);
	(void)window;
}

static void callback_char(GLFWwindow *window, unsigned int codepoint)
{
	event_char(codepoint);
	(void)window;
}

static void callback_mouse(GLFWwindow *window, double pos_x, double pos_y)
{
	event_mouse(pos_x, pos_y);
	(void)window;
}

static void callback_button(GLFWwindow *window, int button, int action, int mods)
{
	event_button(button, action, mods);
	(void)window;
}

static void callback_scroll(GLFWwindow *window, double offset_x, double offset_y)
{
	event_scroll(offset_x, offset_y);
	(void)window;
}

#ifdef DEBUG
static void callback_error(int error, const char *description)
{
	log_error("%s", description);
	(void)error, (void)description;
}
#endif

static void _update_size(void)
{
	GameEngine.Graphics.AspectRatio = (float)GameEngine.Graphics.Width / (float)GameEngine.Graphics.Height;
	renderer2d_dimensions(GameEngine.Graphics.Renderer2D.ProjectionWindow, GameEngine.Graphics.Width, GameEngine.Graphics.Height);
	renderer3d_fbo_destroy();
	renderer3d_dimensions(GameEngine.Graphics.Width, GameEngine.Graphics.Height);
}

