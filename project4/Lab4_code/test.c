#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include "counter.h"
#include "list.h"
#include "hash.h"
#include "mylocks.h"

#define COUNTER      0
#define LIST         1
#define HASH         2

#define BUILD_TYPE   0

#if BUILD_TYPE==COUNTER
	counter_t *c;
#elif BUILD_TYPE==LIST
	list_t *list;
#else 
	hash_t *hash;
#endif

pthread_t *p;

long gettime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
    return tv.tv_sec*1000000 + tv.tv_usec;
}

void *mythread(void *arg) {
	int num=*(int*)arg;
	int tid=(int)syscall(SYS_gettid);
	//int begin_time=gettime();
	while(num--)
	{
		#if BUILD_TYPE == COUNTER
			counter_increment(c,tid);
		#elif BUILD_TYPE == LIST
			list_insert(list,num,tid);
		#else
			//hash_insert(hash,rand(),tid);//随机插入
			hash_insert(hash,num,tid);//每个桶均匀插入
			//hash_insert(hash,320,tid);//只在一个桶里插入，相当于list
		#endif
	}
	/*计算每个线程需要的时间*/
	//int time=gettime()-begin_time;
	//printf("%d : %.3f\n",tid,(float)time/1000000);
	//printf("%d\n",counter_get_value(c,tid));
	return NULL;
}

int main()
{
	#if BUILD_TYPE == COUNTER
		freopen("counter.txt", "a", stdout);
	#elif BUILD_TYPE == LIST
		freopen("list.txt", "a", stdout);
	#else
		freopen("hash.txt", "a", stdout);
	#endif
	
	#if BUILD_TYPE == COUNTER
		printf("counter ");
	#elif BUILD_TYPE == LIST
		printf("list ");
	#else
		printf("hash ");
	#endif

	#if LOCK_TYPE==SPINLOCK
		printf("spinlock:\n");
	#elif LOCK_TYPE==MUTEX
		printf("mutex:\n");
	#elif LOCK_TYPE==FAIRMUTEX
		printf("fairmutex:\n");
	#elif LOCK_TYPE==TWO_PHASE
		printf("two_phase:\n");
	#else
		printf("pthread:\n");
	#endif
	

	int thread_num=20,i;
	//int bucket_num=20;//hash的桶数，默认为20
	int num=1000000;//进行操作的次数
	int start_time,time;
	//for(bucket_num=10;bucket_num<=100;bucket_num+=10)//桶数10-100
	//for(bucket_num=10;bucket_num<=10000000;bucket_num*=10)//桶数10-10000000
	//for(num=10;num<=1000000;num*=10)//操作次数10-1000000
	for(thread_num=1;thread_num<=20;thread_num++)//线程数1-20
	{
		start_time=gettime();
		#if BUILD_TYPE == COUNTER
			c = (counter_t*)malloc(sizeof(counter_t));
			counter_init(c,0);
		#elif BUILD_TYPE == LIST
			list = (list_t*)malloc(sizeof(list_t));
			list_init(list);
		#else
			hash = (hash_t*)malloc(sizeof(hash_t));
			hash_init(hash,20);//默认20
			//hash_init(hash,bucket_num);
		#endif
		
		p = (pthread_t*)malloc(sizeof(pthread_t)*thread_num);
		for(i=0;i<thread_num;i++)
		{
			pthread_create(p+i,NULL,mythread,&num);
		}
		for(i=0;i<thread_num;i++)
		{
			pthread_join(*(p+i),NULL);
		}
		/*计算只有插入操作的时间*/
		time=gettime()-start_time;
		printf("%.3f ",(float)time/1000000);

		#if LOCK_TYPE == PTHREAD && BUILD_TYPE ==COUNTER
		pthread_mutex_destroy(&c->lock);
		#elif LOCK_TYPE == PTHREAD && BUILD_TYPE ==LIST
		pthread_mutex_destroy(&list->lock);
		#endif
	
		#if LOCK_TYPE == PTHREAD && TEST_TYPE == HASH
			list_t *index = hash->lists;
			int i;
			for (i = 0; i < hash->size; ++i) {
				list_t *cur = index + i;
				pthread_mutex_destroy(&cur->lock);
			}
		#endif

		#if BUILD_TYPE == LIST
			list_free(list);
		#endif

		#if BUILD_TYPE == HASH
			hash_free(hash);
		#endif
		/*计算插入和删除所有操作的时间*/
		//time=gettime()-start_time;
		//printf("%.3f ",(float)time/1000000);
		free(p);
	}
	printf("\n");
	
    return 0;
}