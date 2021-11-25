#include "hashmap.h"
#include "crc.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE     256
#define MAX_CHAIN_LENGTH   8

static u32 hashmap_hash(u32 hash, u32 range);
static int _hashmap_rehash(HashMap *hashmap);

int hashmap_init(HashMap *hashmap)
{
	hashmap->Size = INITIAL_SIZE;
	hashmap->Used = 0;
	hashmap->List = calloc(hashmap->Size, sizeof(HashMapNode));
	return 0;
}

int hashmap_insert(HashMap *hashmap, const char *restrict key, const char *restrict value)
{
	u32 i, index;
	index = hashmap_hash(crc32_str(key), hashmap->Size);
	for(i = 0; i < MAX_CHAIN_LENGTH; ++i)
	{
		if(hashmap->List[index].Key)
		{
			if(!strcmp(hashmap->List[index].Key, key))
			{
				return -1;
			}
		}
		else
		{
			++hashmap->Used;
			if(hashmap->Used > (hashmap->Size / 2))
			{
				_hashmap_rehash(hashmap);
			}

			hashmap->List[index].Key = key;
			hashmap->List[index].Value = value;
			return 0;
		}

		index = (index + 1) % hashmap->Size;
	}

	_hashmap_rehash(hashmap);
	if(hashmap_insert(hashmap, key, value) < 0)
	{
		return -1;
	}

	return 0;
}

int hashmap_replace(HashMap *hashmap, const char *restrict key, const char *restrict value)
{
	int index;
	if((index = hashmap_index_of(hashmap, key)) < 0)
	{
		return -1;
	}

	hashmap->List[index].Value = value;
	return 0;
}

int hashmap_index_of(HashMap *hashmap, const char *key)
{
	u32 index, i;
	index = hashmap_hash(crc32_str(key), hashmap->Size);
	for(i = 0; i < MAX_CHAIN_LENGTH; ++i)
	{
		if(hashmap->List[index].Key)
		{
			if(!strcmp(hashmap->List[index].Key, key))
			{
				return (int)index;
			}
		}

		index = (index + 1) % hashmap->Size;
	}

	return -1;
}

int hashmap_remove(HashMap *hashmap, const char *key)
{
	int index;
	if((index = hashmap_index_of(hashmap, key)) < 0)
	{
		return -1;
	}

	hashmap->List[index].Key = NULL;
	--hashmap->Used;
	return 0;
}

void hashmap_destroy(HashMap *hashmap)
{
	free(hashmap->List);
}

static int _hashmap_rehash(HashMap *hashmap)
{
	u32 i, size;
	HashMapNode *nodes;
	nodes = hashmap->List;
	size = hashmap->Size;
	hashmap->Size *= 2;
	hashmap->Used = 0;
	if(!(hashmap->List = calloc(hashmap->Size, sizeof(HashMapNode))))
	{
		return -1;
	}

	for(i = 0; i < size; ++i)
	{
		if(!nodes[i].Key)
		{
			continue;
		}

		if(hashmap_insert(hashmap, nodes[i].Key, nodes[i].Value))
		{
			return -1;
		}
	}

	free(nodes);
	return 0;
}

static u32 hashmap_hash(u32 hash, u32 range)
{
	hash += (hash << 12);
	hash ^= (hash >> 22);
	hash += (hash << 4);
	hash ^= (hash >> 9);
	hash += (hash << 10);
	hash ^= (hash >> 2);
	hash += (hash << 7);
	hash ^= (hash >> 12);
	hash = ((hash >> 3) * 2654435761UL);
	return (u32)(hash % range);
}

