#include "settings.h"
#include "conversion.h"
#include "config.h"
#include "log.h"
#include <stdio.h>

static float _value_float(Config *config, SettingFloat *sf);
static int _value_int(Config *config, SettingInt *si);
static int _value_bool(Config *config, SettingBool *sb);
static const char *_value_string(Config *config, SettingString *ss);

int settings_read(const char *restrict filename, Settings *restrict settings)
{
	unsigned int i;
	Config config;
	if(config_load(filename, &config))
	{
		log_info("Using default values");
		for(i = 0; i < settings->NumFloats; ++i)
		{
			settings->Floats[i].Value = settings->Floats[i].Default;
		}

		for(i = 0; i < settings->NumInts; ++i)
		{
			settings->Ints[i].Value = settings->Ints[i].Default;
		}

		for(i = 0; i < settings->NumBools; ++i)
		{
			settings->Bools[i].Value = settings->Bools[i].Default;
		}

		for(i = 0; i < settings->NumStrings; ++i)
		{
			settings->Strings[i].Value = settings->Strings[i].Default;
		}

		return 0;
	}

	for(i = 0; i < settings->NumFloats; ++i)
	{
		settings->Floats[i].Value = _value_float(&config, &settings->Floats[i]);
	}

	for(i = 0; i < settings->NumInts; ++i)
	{
		settings->Ints[i].Value = _value_int(&config, &settings->Ints[i]);
	}

	for(i = 0; i < settings->NumBools; ++i)
	{
		settings->Bools[i].Value = _value_bool(&config, &settings->Bools[i]);
	}

	for(i = 0; i < settings->NumStrings; ++i)
	{
		settings->Strings[i].Value = _value_string(&config, &settings->Strings[i]);
	}

	config_destroy(&config);
	return 0;
}

int settings_write(const char *restrict filename, Settings *restrict settings)
{
	FILE *fp;
	unsigned int i;
	if(!(fp = fopen(filename, "w")))
	{
		return 1;
	}

	if(fprintf(fp, "; Configuration file - use ingame settings\n") < 0)
	{
		goto fail;
	}

	for(i = 0; i < settings->NumFloats; ++i)
	{
		if(fprintf(fp, "%s=%.4f\n", settings->Floats[i].Key, settings->Floats[i].Value) < 0)
		{
			goto fail;
		}
	}

	for(i = 0; i < settings->NumInts; ++i)
	{
		if(fprintf(fp, "%s=%d\n", settings->Ints[i].Key, settings->Ints[i].Value) < 0)
		{
			goto fail;
		}
	}

	for(i = 0; i < settings->NumBools; ++i)
	{
		if(fprintf(fp, "%s=%s\n", settings->Bools[i].Key, settings->Bools[i].Value ? "true" : "false") < 0)
		{
			goto fail;
		}
	}

	for(i = 0; i < settings->NumStrings; ++i)
	{
		if(fprintf(fp, "%s=%s\n", settings->Strings[i].Key, settings->Strings[i].Value) < 0)
		{
			goto fail;
		}
	}

	fclose(fp);
	return 0;

fail:
	fclose(fp);
	return 1;
}

static float _value_float(Config *config, SettingFloat *sf)
{
	float n;
	const char *p;
	if(!(p = config_get(config, sf->Key)))
	{
		return sf->Default;
	}

	if(parse_float(p, &n))
	{
		return sf->Default;
	}

	return n;
}

static int _value_int(Config *config, SettingInt *si)
{
	int n;
	const char *p;
	if(!(p = config_get(config, si->Key)))
	{
		return si->Default;
	}

	if(parse_int(p, &n))
	{
		return si->Default;
	}

	return n;
}

static int _value_bool(Config *config, SettingBool *sb)
{
	int n;
	const char *p;
	if(!(p = config_get(config, sb->Key)))
	{
		return sb->Default;
	}

	if(parse_bool(p, &n))
	{
		return sb->Default;
	}

	return n;
}

static const char *_value_string(Config *config, SettingString *ss)
{
	const char *p;
	if(!(p = config_get(config, ss->Key)))
	{
		return ss->Default;
	}

	return p;
}

