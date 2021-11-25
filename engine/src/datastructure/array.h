#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "types.h"

/* Resizable array */
#define ARRAY_HEADER(TNAME, FNAME, TYPE)\
typedef struct\
{\
	u32 Length, Capacity;\
	TYPE *Elements;\
} Array##TNAME;\
\
int array_##TYPE##_init(Array##TNAME *array, u32 initial_capacity);\
int array_##TYPE##_insert(Array##TNAME *array, u32 position, TYPE value);\
int array_##TYPE##_insert_multi(Array##TNAME *array, u32 position, TYPE *values, u32 count);\
void array_##TYPE##_delete(Array##TNAME *array, u32 position);\
void array_##TYPE##_delete_multi(Array##TNAME *array, u32 position, u32 count);\
void array_##TYPE##_destroy(Array##TNAME *array);

#define ARRAY_IMPLEMENTATION(TNAME, FNAME, TYPE)\
int array_##FNAME##_init(Array##TNAME *array, u32 initial_capacity)\
{\
	array->Length = 0;\
	array->Capacity = initial_capacity;\
	if(!(array->Elements = malloc(array->Capacity * sizeof(TYPE))))\
	{\
		return -1;\
	}\
\
	return 0;\
}\
\
int array_##FNAME##_insert(Array##TNAME *array, u32 position, TYPE value)\
{\
	return array_##TYPE##_insert_multi(array, position, &value, 1);\
}\
\
int array_##FNAME##_insert_multi(Array##TNAME *array, u32 position, TYPE *values, u32 count)\
{\
	if(array->Length + count >= array->Capacity)\
	{\
		array->Capacity *= 2;\
		TYPE *elems;\
		if((elems = realloc(array->Elements, array->Capacity * sizeof(TYPE))))\
		{\
			return -1;\
		}\
\
		array->Elements = elems;\
	}\
\
	memmove(array->Elements + position + count, array->Elements + position, (array->Length - position) * sizeof(TYPE));\
	memcpy(array->Elements + position, values, count * sizeof(TYPE));\
	array->Length += count;\
	return 0;\
}\
\
void array_##FNAME##_delete(Array##TNAME *array, u32 position)\
{\
	array_##TYPE##_delete_multi(array, position, 1);\
}\
void array_##FNAME##_delete_multi(Array##TNAME *array, u32 position, u32 count)\
{\
	memmove(array->Elements + position, array->Elements + position + count, count * sizeof(TYPE));\
	array->Length -= count;\
}\
\
void array_##FNAME##_destroy(Array##TNAME *array)\
{\
	free(array->Elements);\
}

#endif

