#ifndef __FOLDER_H__
#define __FOLDER_H__

int folder_exists(const char *dirname);
int folder_create(const char *dirname);
int folder_create_if_not_exists(const char *dirname);

#endif

