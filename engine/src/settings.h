#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "types.h"

typedef struct SETTING_INT
{
	const char *Key;
	int Default, Value;
} SettingInt;

typedef struct SETTING_BOOL
{
	const char *Key;
	int Default, Value;
} SettingBool;

typedef struct SETTING_FLOAT
{
	const char *Key;
	float Default, Value;
} SettingFloat;

typedef struct SETTING_STRING
{
	const char *Key, *Default, *Value;
} SettingString;

typedef struct SETTINGS
{
	u32 NumFloats, NumInts, NumBools, NumStrings;
	SettingFloat *Floats;
	SettingInt *Ints;
	SettingBool *Bools;
	SettingString *Strings;
} Settings;

int settings_read(const char *restrict filename, Settings *restrict settings);
int settings_write(const char *restrict filename, Settings *restrict settings);

#endif

