#ifndef __LIST_H_
#define __LIST_H_

#include "mylocks.h"
#include <pthread.h>

typedef struct __node_t {
	int key;
	struct __node_t* next;
} node_t;

typedef struct __list_t {
	node_t *head;
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
}list_t;

void list_init(list_t *list);
void list_insert(list_t *list, unsigned int key,int tid);
void list_delete(list_t *list, unsigned int key,int tid);
void *list_lookup(list_t *list, unsigned int key,int tid);
void list_free(list_t *list);

#endif