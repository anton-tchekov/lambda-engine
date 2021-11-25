#include "readfile.h"
#include <stdio.h>
#include <stdlib.h>

#define READFILE_CHUNK (16 * 1024)

char *readfile(const char *restrict filename, u32 *restrict length)
{
	FILE *in;
	char *data = NULL, *temp;
	u32 size = 0, used = 0, n;
	if(!(in = fopen(filename, "r")))
	{
		return NULL;
	}

	for(;;)
	{
		if(used + READFILE_CHUNK + 1 > size)
		{
			size = used + READFILE_CHUNK + 1;
			if(!(temp = realloc(data, size)))
			{
				free(data);
				fclose(in);
				return 0;
			}

			data = temp;
		}

		if(!(n = (u32)fread(data + used, 1, READFILE_CHUNK, in)))
		{
			break;
		}

		used += n;
	}

	if(ferror(in))
	{
		fclose(in);
		free(data);
		return 0;
	}

	if(!(temp = realloc(data, used + 1)))
	{
		fclose(in);
		free(data);
		return 0;
	}

	data = temp;
	data[used] = '\0';
	*length = used;
	fclose(in);
	return data;
}

