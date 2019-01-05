#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "xchg.c"
#include "sys_futex.c"
#include "mylocks.h"

/*spinlock*/
void spinlock_init(spinlock_t *lock)
{
	lock->flag=0;//0->lock is free
}

void spinlock_acquire(spinlock_t *lock)
{
	while(xchg((volatile unsigned int *)&lock->flag,1)==1)
		;
}

void spinlock_release(spinlock_t *lock)
{
	lock->flag=0;
}


/*mutex*/
void mutex_init(mutex_t *lock)
{
	lock->flag=0;//0->lock is free
}

void mutex_acquire(mutex_t *lock)
{
	if(xchg((volatile unsigned int *)&lock->flag,1)==1)//lock->flag==1
	{
		sys_futex(&lock->flag, FUTEX_WAIT, 1, NULL, 0, 0);
		mutex_acquire(lock);
	}
}

void mutex_release(mutex_t *lock)
{
	lock->flag=0;
	sys_futex(&lock->flag, FUTEX_WAKE, 1, 0, 0, 0);
}


/*fairmutex公平锁，在mutex的基础上加入队列，此处队列采用循环列表*/
/*pBase指向该列表的头*/
/*front是头位置，用来出队；rear是尾位置，用来入队*/
void queue_init(queue_t *q)
{
	q->pBase=(int *)malloc(sizeof(int)*20);
	q->front=0;
	q->rear=0;
}

void queue_add(queue_t *q,int tid)
{
	q->pBase[q->rear]=tid;
	q->rear=(q->rear+1)%20;
}

int queue_remove(queue_t *q)
{	
	int res= q->pBase[q->front];
	q->front=(q->front+1)%20;
	return res;
}

int isEmpty(queue_t *q)
{
	if(q->front==q->rear)
		{
		return 1;
		}
	else 
		return 0;
}

void fairmutex_init(fairmutex_t *lock)
{
	lock->flag=0;//0->lock is free
	lock->guard=0;
	queue_init(&lock->q); 
}

void fairmutex_acquire(fairmutex_t *lock , int tid)
{
	while(xchg((volatile unsigned int *)&lock->guard,1)==1)
		;
	if(lock->flag==0)//lock->flag==1
	{
		lock->flag=1;
		lock->guard=0;
	}else{
		queue_add(&lock->q,tid);
		lock->guard=0; 
		sys_futex(&lock->flag, FUTEX_WAIT_BITSET, 1, NULL, 0, tid);
	}
}

void fairmutex_release(fairmutex_t *lock)
{
	while(xchg((volatile unsigned int *)&lock->guard,1)==1)
		;
	if(isEmpty(&lock->q))
		lock->flag=0;
	else{
		int tid=queue_remove(&lock->q);
	    sys_futex(&lock->flag, FUTEX_WAKE_BITSET, 1, 0, 0, tid);
	}
	lock->guard=0;
}


/*two_phase两相锁，先自旋一段时间再睡觉*/
/*代码参考pdf第28章19页*/
void two_phase_init(two_phase_t *lock)
{
	lock->flag=0;//0->lock is free
	lock->guard=0;
	lock->count=0;
}

void two_phase_acquire(two_phase_t *lock)
{
	if(xchg((volatile unsigned int *)&lock->flag,1)==1)
	{
		while(xchg((volatile unsigned int *)&lock->guard,1)==1)
			;
		lock->count++;
		lock->guard=0;
		while(1){
			if(xchg((volatile unsigned int *)&lock->flag,1)==0)
			{
				while(xchg((volatile unsigned int *)&lock->guard,1)==1)
					;
				lock->count--;
				lock->guard=0;
				return;
			}
			if(lock->flag==0)
				continue;
			sys_futex(&lock->flag, FUTEX_WAIT, 1, NULL, 0, 0);
		}
	}
}

void two_phase_release(two_phase_t *lock)
{
	lock->flag=0;
	sys_futex(&lock->flag, FUTEX_WAKE, lock->count, 0, 0, 0);
}


/*五种锁的整合，方便直接调用*/
void lock_init(void *lock)
{
	#if LOCK_TYPE==SPINLOCK
		spinlock_init((spinlock_t *)lock);
	#elif LOCK_TYPE==MUTEX
		mutex_init((mutex_t *)lock);
	#elif LOCK_TYPE==FAIRMUTEX
		fairmutex_init((fairmutex_t *)lock);
	#elif LOCK_TYPE==TWO_PHASE
		two_phase_init((two_phase_t *)lock);
	#else
		pthread_mutex_init((pthread_mutex_t*)lock,NULL);
	#endif
}

void lock_acquire(void *lock,int tid)
{
	#if LOCK_TYPE==SPINLOCK
		spinlock_acquire((spinlock_t *)lock);
	#elif LOCK_TYPE==MUTEX
		mutex_acquire((mutex_t *)lock);
	#elif LOCK_TYPE==FAIRMUTEX
		fairmutex_acquire((fairmutex_t *)lock,tid);
	#elif LOCK_TYPE==TWO_PHASE
		two_phase_acquire((two_phase_t *)lock);
	#else 
		pthread_mutex_lock((pthread_mutex_t *)lock); 
	#endif
}

void lock_release(void *lock)
{
	#if LOCK_TYPE==SPINLOCK
		spinlock_release((spinlock_t *)lock);
	#elif LOCK_TYPE==MUTEX
		mutex_release((mutex_t *)lock);
	#elif LOCK_TYPE==FAIRMUTEX
		fairmutex_release((fairmutex_t *)lock);
	#elif LOCK_TYPE==TWO_PHASE
		two_phase_release((two_phase_t *)lock);
	#else
		pthread_mutex_unlock((pthread_mutex_t*)lock);
	#endif
}
