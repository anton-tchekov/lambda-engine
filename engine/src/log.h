#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>

int log_init(void);
int log_output(const char *file);

#ifdef DEBUG

void log_trace(const char *format, ...);
void log_debug(const char *format, ...);

#else

#define log_trace(...)
#define log_debug(...)

#endif

void log_info(const char *format, ...);
void log_warn(const char *format, ...);
void log_error(const char *format, ...);
void log_fatal(const char *format, ...);

#endif

