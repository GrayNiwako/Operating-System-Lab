#include <stdio.h>
#include <stdlib.h>
#include "mylocks.h"
#include "hash.h"
#include "list.h"

void hash_init(hash_t *hash, int size)
{
	hash->size=size;
	hash->lists=(list_t*)malloc(sizeof(list_t)*size);
	int i;
	for(i=0;i<size;i++){
		list_init(&hash->lists[i]);
	}
}

void hash_insert(hash_t *hash, unsigned int key,int tid)
{
	int bucket=key%hash->size;
	list_insert(&hash->lists[bucket], key,tid);
}

void hash_delete(hash_t *hash, unsigned int key,int tid)
{
	int bucket=key%hash->size;
	list_delete(&hash->lists[bucket], key,tid);
}

void *hash_lookup(hash_t *hash, unsigned int key,int tid)
{
	int bucket=key%hash->size;
	return list_lookup(&hash->lists[bucket], key,tid);
}

void hash_free(hash_t *hash)
{
	int i;
	for(i=0;i<hash->size;i++)
		list_free(&hash->lists[i]);
}