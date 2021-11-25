#ifndef __ACTION_COND_H__
#define __ACTION_COND_H__

#include "types.h"

u32 condition_action(int (*condition)(void));
void condition_reset(void);
int condition_get(u32 n);

#endif
