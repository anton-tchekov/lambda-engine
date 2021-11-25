#include <stdio.h>
#include "shader.h"
#include "log.h"
#include "debug.h"
#include <GLFW/glfw3.h>

int main(int argc, char **argv)
{
	GLuint program;
	GLFWwindow *window;

	if(log_init())
	{
		return 1;
	}

	if(argc != 3 && argc != 4)
	{
		fprintf(stderr, "Usage:\n"
			"./shader_cheker \"vertex shader\" \"fragment shader\"\n"
			"./shader_cheker \"vertex shader\" \"geometry shader\" \"fragment shader\"\n");
		return 1;
	}

	if(!glfwInit())
	{
		return 1;
	}

	/* OpenGL Version 3.3 Core Profile */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Double buffering */
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	/* Hide window */
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	/* Maximized window */
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	/* Forward compatibility context */
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/* Create window */
	if(!(window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL)))
	{
		fprintf(stderr, "Creating window failed\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	/* Enable VSync */
	glfwSwapInterval(1);

	/* Load opengl functions */
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Loading OpenGL functions failed\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	if(argc == 3)
	{
		if(!(program = shader_load_file_vf(argv[1], argv[2])))
		{
			fprintf(stderr, "Compilation failed\n");
			return 1;
		}
	}
	else if(argc == 4)
	{
		if(!(program = shader_load_file_vgf(argv[1], argv[2], argv[3])))
		{
			fprintf(stderr, "Compilation failed\n");
			return 1;
		}
	}

	fprintf(stderr, "Compilation succeeded\n");
	GL_CHECK(glDeleteProgram(program));
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

