#ifndef __BLUR_H__
#define __BLUR_H__

#include "opengl.h"

int gblur_init(void);
void gblur_destroy(void);

/* Gaussian blur a texture in place */
void gblur5(void);

#endif

