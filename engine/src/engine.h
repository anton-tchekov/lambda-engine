#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "types.h"

#define ENGINE_ENABLE_AUDIO   1
#define ENGINE_ENABLE_NETWORK 2

int engine_init(int flags, const char *title, int min_w, int min_h);
void engine_main_loop(void);
void engine_destroy(void);
void engine_update(void);

void engine_exit(void);
u32 engine_fps(void);
double engine_time(void);
double engine_delta(void);

#endif

