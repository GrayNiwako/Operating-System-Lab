#ifndef __MYLOCKS_H_
#define __MYLOCKS_H_

#define SPINLOCK   0
#define MUTEX      1
#define TWO_PHASE  2
#define PTHREAD    3
#define FAIRMUTEX  4

#define LOCK_TYPE  0

typedef struct __spinlock_t {
	int flag;
}spinlock_t;

typedef struct __mutex_t {
	int flag;
}mutex_t;

typedef struct __queue_t {
	int *pBase;
	int front;
	int rear;
}queue_t;

typedef struct __fairmutex_t {
	int flag;
	int guard;
	queue_t q;
}fairmutex_t;

typedef struct __two_phase_t {
	int flag;
	int guard;
	int count;
}two_phase_t;

void spinlock_init(spinlock_t *lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

void mutex_init(mutex_t *lock);
void mutex_acquire(mutex_t *);
void mutex_release(mutex_t *lock);

void queue_init(queue_t *q);
void queue_add(queue_t *q,int tid);
int queue_remove(queue_t *q);
int isEmpty(queue_t *q);
void fairmutex_init(fairmutex_t *lock);
void fairmutex_acquire(fairmutex_t *lock,int tid);
void fairmutex_release(fairmutex_t *lock);

void two_phase_init(two_phase_t *lock);
void two_phase_acquire(two_phase_t *lock);
void two_phase_release(two_phase_t *lock);

void lock_init(void *lock);
void lock_acquire(void *lock,int tid);
void lock_release(void *lock);

#endif