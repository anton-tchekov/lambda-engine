#include "debug.h"
#include "internal.h"

#ifdef DEBUG

static const char *_unknown_error = "Unknown error";

static const char *_gl_error_string(GLenum code)
{
	switch(code)
	{
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";

		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";

		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";

		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
	}

	return _unknown_error;
}

static const char *_al_error_string(ALenum code)
{
	switch(code)
	{
		case AL_INVALID_NAME:
			return "AL_INVALID_NAME";

		case AL_INVALID_ENUM:
			return "AL_INVALID_ENUM";

		case AL_INVALID_VALUE:
			return "AL_INVALID_VALUE";

		case AL_INVALID_OPERATION:
			return "AL_INVALID_VALUE";

		case AL_OUT_OF_MEMORY:
			return "AL_OUT_OF_MEMORY";
	}

	return _unknown_error;
}

static const char *_alc_error_string(ALCenum code)
{
	switch(code)
	{
		case ALC_INVALID_DEVICE:
			return "ALC_INVALID_DEVICE";

		case ALC_INVALID_CONTEXT:
			return "ALC_INVALID_CONTEXT";

		case ALC_INVALID_ENUM:
			return "ALC_INVALID_ENUM";

		case ALC_INVALID_VALUE:
			return "ALC_INVALID_VALUE";

		case ALC_OUT_OF_MEMORY:
			return "ALC_OUT_OF_MEMORY";
	}

	return _unknown_error;
}

void gl_check(const char *file, int line, const char *function)
{
	GLenum gl_err;
	while((gl_err = glGetError()) != GL_NO_ERROR)
	{
		log_error("%s:%d\n%s;\nGL_ERROR 0x%x: %s\n", file, line, function, gl_err, _gl_error_string(gl_err));
	}
}

void al_check(const char *file, int line, const char *function)
{
	ALenum al_err;
	while((al_err = alGetError()) != AL_NO_ERROR)
	{
		log_error("%s:%d\n%s;\nAL_ERROR 0x%x: %s\n", file, line, function, al_err, _al_error_string(al_err));
	}
}

void alc_check(const char *file, int line, const char *function)
{
	ALCenum alc_err;
	while((alc_err = alcGetError(GameEngine.Audio.Device)) != ALC_NO_ERROR)
	{
		log_error("%s:%d\n%s;\nALC_ERROR 0x%x: %s\n", file, line, function, alc_err, _alc_error_string(alc_err));
	}
}

#endif

