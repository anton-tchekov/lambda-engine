#include "thread.h"

#define MAX_THREADS 16

static pthread_t _thread_map[MAX_THREADS];
static u32 _thread_index;

u32 thread_add(pthread_t thread)
{
	if(_thread_index >= MAX_THREADS - 1)
	{
		return 1;
	}

	_thread_map[_thread_index++] = thread;
	return 0;
}

u32 thread_id(pthread_t thread)
{
	u32 i;
	for(i = 0; i < _thread_index; ++i)
	{
		if(pthread_equal(thread, _thread_map[i]))
		{
			return i;
		}
	}

	return 0;
}

int thread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg)
{
	int rv;
	rv = pthread_create(thread, attr, start_routine, arg);
	if(thread_add(*thread))
	{
		return -1;
	}

	return rv;
}

