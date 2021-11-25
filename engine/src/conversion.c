#include "conversion.h"
#include <stdlib.h>
#include <string.h>

int parse_int(const char *s, int *out)
{
	char *e;
	*out = (int)strtol(s, &e, 0);
	return *e;
}

int parse_float(const char *s, float *out)
{
	char *e;
	*out = strtof(s, &e);
	return *e;
}

int parse_bool(const char *s, int *out)
{
	if(!strcmp(s, "true"))
	{
		*out = 1;
		return 0;
	}

	if(!strcmp(s, "false"))
	{
		*out = 0;
		return 0;
	}

	return 1;
}

