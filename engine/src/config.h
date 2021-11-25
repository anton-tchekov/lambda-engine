#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "hashmap.h"

typedef struct CONFIG
{
	char *File;
	HashMap HashMap;
} Config;

int config_load(const char *filename, Config *config);
const char *config_get(Config *config, const char *key);
void config_destroy(Config *config);

#endif

