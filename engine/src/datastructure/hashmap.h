#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "types.h"

typedef struct
{
	const char *Key, *Value;
} HashMapNode;

typedef struct HASH_MAP
{
	u32 Size, Used;
	HashMapNode *List;
} HashMap;

int hashmap_init(HashMap *hashmap);
int hashmap_insert(HashMap *restrict hashmap, const char *restrict key, const char *restrict value);
int hashmap_index_of(HashMap *restrict hashmap, const char *restrict key);
int hashmap_replace(HashMap *restrict hashmap, const char *restrict key, const char *restrict value);
int hashmap_remove(HashMap *restrict hashmap, const char *restrict key);
void hashmap_destroy(HashMap *hashmap);

#endif

