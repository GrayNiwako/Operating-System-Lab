#include <stdio.h>
#include <stdlib.h>
#include "mylocks.h"
#include "list.h"

void list_init(list_t *list)
{
	list->head = NULL;
	lock_init((void*)&list->lock);
}

void list_insert(list_t *list, unsigned int key,int tid)
{
	lock_acquire((void*)&list->lock,tid);
	node_t *new=(node_t*)malloc(sizeof(node_t));
	if(new==NULL)
	{
		perror("malloc");
		lock_release((void*)&list->lock);
	}
	new->key=key;
	new->next=list->head;
	list->head=new;
	lock_release((void*)&list->lock);
}

void list_delete(list_t *list, unsigned int key,int tid)
{
	lock_acquire((void*)&list->lock,tid);
	node_t *curr=list->head;
	node_t *p=NULL;
	while(curr)
	{
		if(curr->key==key) break;
		p=curr;
		curr=curr->next;
	}
	if(curr==NULL){
		lock_release((void*)&list->lock);
		return;
	}
	if(p==NULL){
		list->head=curr->next;
		free(curr);
	}else{
		p->next=curr->next;
		free(curr);
	}
	lock_release((void*)&list->lock);
}

void *list_lookup(list_t *list, unsigned int key,int tid)
{
	lock_acquire((void*)&list->lock,tid);
	node_t *curr=list->head;
	while(curr)
	{
		if(curr->key==key)
		{
			lock_release((void*)&list->lock);
			return (void *)curr;
		}
		curr=curr->next;
	}
	lock_release((void*)&list->lock);
	return NULL;
}

void list_free(list_t *list) 
{
	node_t *curr=list->head;
	while(curr)
	{
		node_t *p=curr;
		curr=curr->next;
		free(p);
	}
}