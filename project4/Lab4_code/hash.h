#ifndef __HASH_H_
#define __HASH_H_

#include "mylocks.h"
#include <pthread.h>
#include "list.h"

typedef struct __hash_t {
	int size;
	list_t *lists;
}hash_t;

void hash_init(hash_t *hash, int size);
void hash_insert(hash_t *hash, unsigned int key,int tid);
void hash_delete(hash_t *hash, unsigned int key,int tid);
void *hash_lookup(hash_t *hash, unsigned int key,int tid);
void hash_free(hash_t *hash);

#endif