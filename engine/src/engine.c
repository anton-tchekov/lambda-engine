#include "engine.h"
#include "internal.h"
#include "log.h"
#include "debug.h"
#include "audio.h"
#include "graphics.h"
#include "renderer2d.h"
#include "renderer3d.h"
#include "billboard.h"
#include "screenshot.h"
#include "network.h"

void event_screenshot(const char *filename);

static void callback_screenshot(const char *filename)
{
	log_debug("Screenshot saved as %s\n", filename);
	event_screenshot(filename);
}

int engine_init(int flags, const char *title, int min_w, int min_h)
{
	if(log_init())
	{
		return 1;
	}

	if(graphics_init(title, min_w, min_h))
	{
		return 1;
	}

	if(flags & ENGINE_ENABLE_AUDIO)
	{
		if(audio_init())
		{
			log_info("Audio will be disabled");
		}
	}

	if(flags & ENGINE_ENABLE_NETWORK)
	{
		if(network_init())
		{
			log_info("Networking will be disabled");
		}
	}

	return 0;
}

void engine_main_loop(void)
{
	glfwShowWindow(GameEngine.Graphics.Window);
	while(!glfwWindowShouldClose(GameEngine.Graphics.Window))
	{
		if(GameEngine.Graphics.EventMode)
		{
			glfwWaitEvents();
		}
		else
		{
			glfwPollEvents();
		}

		GameEngine.CurrentTime = glfwGetTime();
		GameEngine.DeltaTime = GameEngine.CurrentTime - GameEngine.LastTime;
		GameEngine.LastTime = GameEngine.CurrentTime;

		++GameEngine.Graphics.Frames;
		if(GameEngine.CurrentTime > (GameEngine.Graphics.FPS_Time + 1.0))
		{
			GameEngine.Graphics.FPS = GameEngine.Graphics.Frames;
			GameEngine.Graphics.FPS_Time = GameEngine.CurrentTime;
			GameEngine.Graphics.Frames = 0;

			GameEngine.Graphics.TEX_PS = GameEngine.Graphics.TEX_CT / GameEngine.Graphics.FPS;
			GameEngine.Graphics.VBO_PS = GameEngine.Graphics.VBO_CT / GameEngine.Graphics.FPS;
			GameEngine.Graphics.VAO_PS = GameEngine.Graphics.VAO_CT / GameEngine.Graphics.FPS;
			GameEngine.Graphics.DRW_PS = GameEngine.Graphics.DRW_CT / GameEngine.Graphics.FPS;
			GameEngine.Graphics.VTX_PS = GameEngine.Graphics.VTX_CT / GameEngine.Graphics.FPS;
			GameEngine.Graphics.FBO_PS = GameEngine.Graphics.FBO_CT / GameEngine.Graphics.FPS;

			GameEngine.Graphics.TEX_CT = 0;
			GameEngine.Graphics.VBO_CT = 0;
			GameEngine.Graphics.VAO_CT = 0;
			GameEngine.Graphics.DRW_CT = 0;
			GameEngine.Graphics.VTX_CT = 0;
			GameEngine.Graphics.FBO_CT = 0;
		}

		if(GameEngine.Graphics.Renderer3D.Camera)
		{
			camera_perspective(GameEngine.Graphics.Renderer3D.Camera,
				GameEngine.Graphics.VR_Mode ?
						GameEngine.Graphics.AspectRatio / 2.0f :
						GameEngine.Graphics.AspectRatio);
		}

		engine_update();

		if(GameEngine.Graphics.Renderer3D.Camera)
		{
			audio_listener_update(GameEngine.Graphics.Renderer3D.Camera);
		}

		/* Store 2D renderer commands */
		renderer2d_prepare();

		/* Render 2D (Billboard/Offscreen) */
		renderer2d_billboard_render();

		/* Store billboard rendering commands */
		if(GameEngine.Graphics.Render3D)
		{
			billboard_prepare();
		}

		/* Render 3D */
		if(!GameEngine.Graphics.Render3D || renderer3d_render())
		{
			GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
		}

		GL_CHECK(glViewport(0, 0, GameEngine.Graphics.Width, GameEngine.Graphics.Height));

		/* Render 2D (Overlay) */
		renderer2d_render();
		if(GameEngine.Graphics.Screenshot)
		{
			GameEngine.Graphics.Screenshot = 0;
			GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
			screenshot(callback_screenshot);
		}

		glfwSwapBuffers(GameEngine.Graphics.Window);
	}

	glfwHideWindow(GameEngine.Graphics.Window);
}

void engine_exit(void)
{
	glfwSetWindowShouldClose(GameEngine.Graphics.Window, 1);
}

int engine_width(void)
{
	return GameEngine.Graphics.Width;
}

int engine_height(void)
{
	return GameEngine.Graphics.Height;
}

u32 engine_fps(void)
{
	return GameEngine.Graphics.FPS;
}

double engine_time(void)
{
	return GameEngine.CurrentTime;
}

double engine_delta(void)
{
	return GameEngine.DeltaTime;
}

void engine_destroy(void)
{
	log_debug("Destroying game engine");
	network_destroy();
	audio_destroy();
	graphics_destroy();
}

