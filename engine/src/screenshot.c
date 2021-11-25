#include "screenshot.h"
#include "types.h"
#include "opengl.h"
#include "thread.h"
#include "bmp.h"
#include "internal.h"
#include "debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define FILENAME_LENGTH 128

typedef struct THREAD_DATA
{
	u8 *Data;
	char *Filename;
	void (*EventComplete)(const char *);
} ThreadData;

static void *thread_write(void *in);

int screenshot(void (*event_complete)(const char *))
{
	static int sid = 0;
	u8 *data;
	time_t t;
	struct tm tm;
	pthread_t wt;
	pthread_attr_t at;
	char *filename;
	ThreadData *td;
	if(!(td = malloc(sizeof(ThreadData))))
	{
		return 1;
	}

	if(!(filename = malloc(FILENAME_LENGTH)))
	{
		free(td);
		return 1;
	}

	if(!(data = malloc(4UL * (size_t)GameEngine.Graphics.Width * (size_t)GameEngine.Graphics.Height)))
	{
		free(td);
		free(filename);
		return 1;
	}

	t = time(NULL);
	tm = *localtime(&t);
	snprintf(filename, FILENAME_LENGTH, "screenshot_%d-%02d-%02d_%02d-%02d-%02d_%06d.bmp",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ++sid);

	GL_CHECK(glReadPixels(0, 0, GameEngine.Graphics.Width, GameEngine.Graphics.Height, GL_BGRA, GL_UNSIGNED_BYTE, data));

	td->Data = data;
	td->Filename = filename;
	td->EventComplete = event_complete;

	if(pthread_attr_init(&at) ||
		pthread_attr_setdetachstate(&at, PTHREAD_CREATE_DETACHED) ||
		thread_create(&wt, NULL, thread_write, td))
	{
		log_error("Failed to create screenshot");
		free(td);
		free(filename);
		free(data);
		return 1;
	}

	return 0;
}

static void *thread_write(void *in)
{
	ThreadData *td = (ThreadData *)in;
	bmp_write(td->Filename, td->Data, GameEngine.Graphics.Width, GameEngine.Graphics.Height);
	td->EventComplete(td->Filename);
	free(td->Filename);
	free(td->Data);
	return NULL;
}

