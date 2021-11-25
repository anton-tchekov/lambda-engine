#include "folder.h"
#include <dirent.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

int folder_exists(const char *dirname)
{
	DIR *dir = opendir(dirname);
	if(dir)
	{
		closedir(dir);
		return 1;
	}
	else
	{
		return 0;
	}
}

int folder_create(const char *dirname)
{
	int error;

#if defined(_WIN32)
	error = _mkdir(dirname);
#else
	{
		mode_t mode = 0755;
		error = mkdir(dirname, mode);
	}
#endif

	return error != 0;
}

int folder_create_if_not_exists(const char *dirname)
{
	if(folder_exists(dirname))
	{
		return 0;
	}

	if(folder_create(dirname))
	{
		return 1;
	}

	return 0;
}

