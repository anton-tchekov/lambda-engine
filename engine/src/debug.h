#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "opengl.h"
#include "openal.h"
#include "log.h"

#ifdef DEBUG

void gl_check(const char *file, int line, const char *function);
void al_check(const char *file, int line, const char *function);
void alc_check(const char *file, int line, const char *function);

#define GL_CHECK(gl_fn) \
	do \
	{ \
		gl_fn; \
		gl_check(__FILE__, __LINE__, #gl_fn); \
	} while(0)

#define AL_CHECK(al_fn) \
	do \
	{ \
		al_fn; \
		al_check(__FILE__, __LINE__, #al_fn); \
	} while(0)

#define ALC_CHECK(alc_fn) \
	do \
	{ \
		alc_fn; \
		alc_check(__FILE__, __LINE__, #alc_fn); \
	} while(0)

#define TIMER_START(name) \
	{ \
		char *__timer_##name = #name; \
		double __timer_##name##_begin = glfwGetTime();

#define TIMER_END(name) \
		log_info("Timer %s delta: %f ms", __timer_##name, (glfwGetTime() - __timer_##name##_begin) * 1000.0); \
	}

#else

#define GL_CHECK(gl_fn) \
	gl_fn

#define AL_CHECK(al_fn) \
	al_fn

#define ALC_CHECK(alc_fn) \
	alc_fn

#define TIMER_START(name)

#define TIMER_END(name)

#endif /* DEBUG */

#endif /* __DEBUG_H__ */

