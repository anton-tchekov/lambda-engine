#ifndef __TRHEAD_H__
#define __THREAD_H__

#include <pthread.h>
#include "types.h"

u32 thread_add(pthread_t thread);
u32 thread_id(pthread_t thread);
int thread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);

#endif

