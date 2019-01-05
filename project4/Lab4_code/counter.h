#ifndef __COUNTER_H_
#define __COUNTER_H_

#include "mylocks.h"
#include <pthread.h>

typedef struct __counter_t {
	int count;
	#if LOCK_TYPE==SPINLOCK
		spinlock_t lock;
	#elif LOCK_TYPE==MUTEX
		mutex_t lock;
	#elif LOCK_TYPE==FAIRMUTEX
		fairmutex_t lock;
	#elif LOCK_TYPE==TWO_PHASE
		two_phase_t lock;
	#else
		pthread_mutex_t lock;
	#endif
}counter_t;

void counter_init(counter_t *c, int value);
int counter_get_value(counter_t *c,int tid);
void counter_increment(counter_t *c,int tid);
void counter_decrement(counter_t *c,int tid);

#endif
