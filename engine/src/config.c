#include "config.h"
#include "readfile.h"
#include <stdlib.h>
#include "log.h"

int config_load(const char *filename, Config *config)
{
	u32 length;
	int line;
	char c, *p, *key, *value;
	if(!(config->File = readfile(filename, &length)))
	{
		log_error("Failed to read configuration file \"%s\"", filename);
		return 1;
	}

	if(hashmap_init(&config->HashMap))
	{
		log_debug("Failed to initialize hashmap");
		free(config->File);
		return 1;
	}

	log_debug("Successfully read config file");
	line = 1;
	p = config->File;
	while((c = *p))
	{
		if(c == '\n')
		{
			goto next;
		}

		if(c == ';')
		{
			while((c = *p) != '\0' && c != '\n')
			{
				++p;
			}

			goto next;
		}

		key = p;
		while((c = *p) != '=')
		{
			if(c == '\n')
			{
				log_error("Parse error on line %d: Unexpected end of line", line);
				goto print;
			}

			if(c == '\0')
			{
				log_error("Parse error on line %d: Unexpected end of file", line);
				goto error;
			}

			++p;
		}

		if(p == key)
		{
			log_error("Parse error on line %d: Key expected", line);
			goto print;
		}

		*p = '\0';
		++p;
		value = p;
		while((c = *p) != '\n' && c != '\0')
		{
			++p;
		}

		*p = '\0';
		if(hashmap_insert(&config->HashMap, key, value))
		{
			log_error("Failed to insert key/value pair (%s/%s) into hashmap", key, value);
			goto error;
		}

next:
		++p;
		++line;
	}

	return 0;

print:
	while((c = *p) != '\0' && c != '\n')
	{
		++p;
	}

	while((c = *p))
	{
		if(c == '\n')
		{
			goto next2;
		}

		if(c == ';')
		{
			while((c = *p) != '\0' && c != '\n')
			{
				++p;
			}

			goto next2;
		}

		key = p;
		while((c = *p) != '=')
		{
			if(c == '\n')
			{
				log_error("Parse error on line %d: Unexpected end of line", line);
				goto print;
			}

			if(c == '\0')
			{
				log_error("Parse error on line %d: Unexpected end of file", line);
				goto error;
			}

			++p;
		}

		if(p == key)
		{
			log_error("Parse error on line %d: Key expected", line);
			goto print;
		}

		++p;
		while((c = *p) != '\n' && c != '\0')
		{
			++p;
		}

next2:
		++p;
		++line;
	}

error:
	hashmap_destroy(&config->HashMap);
	free(config->File);
	return -1;
}

const char *config_get(Config *config, const char *key)
{
	int idx;
	if((idx = hashmap_index_of(&config->HashMap, key)) < 0)
	{
		return NULL;
	}

	return config->HashMap.List[idx].Value;
}

void config_destroy(Config *config)
{
	hashmap_destroy(&config->HashMap);
	free(config->File);
}

