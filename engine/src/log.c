#include "log.h"
#include "thread.h"
#include "terminal.h"
#include "opengl.h"
#include <stdio.h>
#include <locale.h>

static FILE *log_file;
static pthread_mutex_t _mutex;

static int _log(const char *level, const char *format, va_list args);

int log_init(void)
{
	setlocale(LC_CTYPE, "");
	log_file = stderr;
	if(pthread_mutex_init(&_mutex, NULL))
	{
		fprintf(stderr, "Error initializing logging mutex\n");
		return -1;
	}

	thread_add(pthread_self());
	return 0;
}

int log_output(const char *file)
{
	if(log_file)
	{
		fclose(log_file);
		log_file = NULL;
	}

	if(file)
	{
		if(!(log_file = fopen(file, "a")))
		{
			log_warn("Failed to open log file \"%s\" for appending; Falling back to stderr", file);
			log_file = stderr;
			return -1;
		}
	}
	else
	{
		log_file = stderr;
	}

	return 0;
}

#ifdef DEBUG

void log_trace(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(log_file == stderr ? "[ " TERMINAL_COLOR_BOLD_CYAN "TRACE" TERMINAL_COLOR_RESET " ]" : "[ TRACE ]", format, args);
	va_end(args);
}

void log_debug(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(log_file == stderr ? "[ " TERMINAL_COLOR_BOLD_GREEN "DEBUG" TERMINAL_COLOR_RESET " ]" : "[ DEBUG ]", format, args);
	va_end(args);
}

#endif

void log_info(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(log_file == stderr ? "[ " TERMINAL_COLOR_BOLD_BLUE "INFO " TERMINAL_COLOR_RESET " ]" : "[ INFO  ]", format, args);
	va_end(args);
}

void log_warn(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(log_file == stderr ? "[ " TERMINAL_COLOR_BOLD_YELLOW "WARN " TERMINAL_COLOR_RESET " ]" : "[ WARN  ]", format, args);
	va_end(args);
}

void log_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(log_file == stderr ? "[ " TERMINAL_COLOR_BOLD_RED "ERROR" TERMINAL_COLOR_RESET " ]" : "[ ERROR ]", format, args);
	va_end(args);
}

void log_fatal(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	_log(log_file == stderr ? "[ " TERMINAL_COLOR_BOLD_MAGENTA "FATAL" TERMINAL_COLOR_RESET " ]" : "[ FATAL ]", format, args);
	va_end(args);
}

static int _log(const char *level, const char *format, va_list args)
{
	double time;
	if(pthread_mutex_lock(&_mutex))
	{
		return -1;
	}

	time = glfwGetTime();
	fprintf(log_file, "[ %8d.%03d ][ T-%02d ]%s ",
			(int)time, (int)(time * 1000.0) % 1000,
			thread_id(pthread_self()), level);

	vfprintf(stderr, format, args);
	fputs("\n", stderr);
	if(pthread_mutex_unlock(&_mutex))
	{
		return -1;
	}

	return 0;
}

