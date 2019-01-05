#include <stdio.h>
#include "mylocks.h"
#include "counter.h"

void counter_init(counter_t *c, int value)
{
	c->count=value;
	lock_init((void*)&c->lock);
}

int counter_get_value(counter_t *c,int tid)
{
	lock_acquire((void*)&c->lock,tid);
	int value=c->count;
	lock_release((void*)&c->lock);
	return value;
}

void counter_increment(counter_t *c,int tid)
{
	lock_acquire((void*)&c->lock,tid);
	c->count++;
	lock_release((void*)&c->lock);
}

void counter_decrement(counter_t *c,int tid)
{
	lock_acquire((void*)&c->lock,tid);
	c->count--;
	lock_release((void*)&c->lock);
}
