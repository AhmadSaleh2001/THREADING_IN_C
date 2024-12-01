

#if 0

Problem Statement :
====================
Write a program which launches 4 threads - 2 consumer threads and 2 producer threads. Threads
are created in JOINABLE Mode.
All 4 threads act on a shared resource - A Queue of integers. Producer threads produce
a random integer and add it to Queue, Consumer threads remove an integer from the Queue.
Maximum size of the Queue is 5.

Following are the constraints applied :

1. When producer threads produce an element and add it to the Queue, it does not release the Queue
untill the Queue is full i.e producer thread release the Queue only when it is full

2. When consumer threads consume an element from the Queue, it consumes the entire Queue and
do not release it until the Queue is empty.

3. Consumer Signals the Producers when Queue is Exhausted, Producers Signals the Consumers when Queue	
becomes full

Guidelines :
Use as many printfs as possible, so you can debug the program easily

Conmpile and Run :
gcc -g -c Queue.c -o Queue.o
gcc -g -c Assignment_prod_cons_on_Q.c -o Assignment_prod_cons_on_Q.o
gcc -g Assignment_prod_cons_on_Q.o Queue.o -o exe -lpthread

#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "Queue.h"

static int new_int()
{

	static int a = 0;
	a++;
	return a;
}

struct Queue_t *Q;

static const char *prod1 = "TP1";
static const char *prod2 = "TP2";
static const char *prod3 = "TP3";
static const char *cons1 = "TC1";
static const char *cons2 = "TC2";
static const char *cons3 = "TC3";

static void *
prod_fn(void *arg)
{

	char *th_name = (char *)arg;

	// STEP 1
	printf("here prod waiting %s\n", th_name);
	pthread_mutex_lock(&Q->q_mutex);

	while (is_queue_full(Q))
	{
		printf("here prod joined %s\n", th_name);
		pthread_cond_wait(&Q->q_cv, &Q->q_mutex);
	}

	int who = th_name[2] - '0';
	while (!is_queue_full(Q))
	{
		printf("Thread: %s is producing %d\n", th_name, who);
		enqueue(Q, (void *)who);
	}
	pthread_cond_signal(&Q->q_cv); // what if we signal the wrong thread? meaing the prducer instead of consumer
	// lets keep trying signal then
	pthread_mutex_unlock(&Q->q_mutex);

	return NULL;
}

static void *
cons_fn(void *arg)
{

	char *th_name = (char *)arg;

	// STEP 2
	printf("here cons waiting %s\n", th_name);
	pthread_mutex_lock(&Q->q_mutex);

	while (is_queue_empty(Q))
	{
		printf("here cons joined %s\n", th_name);
		pthread_cond_wait(&Q->q_cv, &Q->q_mutex);
	}

	int who = th_name[2] - '0';
	while (!is_queue_empty(Q))
	{
		int *element = deque(Q);
		printf("Thread: %s is consuming %d\n", th_name, element);
	}
	pthread_cond_signal(&Q->q_cv); // what if we signal the wrong thread? meaing the consumer instead of producer
	// lets keep trying signal then
	pthread_mutex_unlock(&Q->q_mutex);

	return NULL;
}

int main(int argc, char **argv)
{

	/* initialize the Queue and its Mutex + CV */
	Q = initQ();

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_t prod_th1, prod_th2, prod_th3; /*  Two producer threads */
	pthread_t cons_th1, cons_th2, cons_th3; /*  Two consumer threads */

	pthread_create(
			&prod_th1, &attr, prod_fn, (void *)prod1);

	pthread_create(
			&prod_th2, &attr, prod_fn, (void *)prod2);

	pthread_create(
			&prod_th3, &attr, prod_fn, (void *)prod3);

	pthread_create(
			&cons_th1, &attr, cons_fn, (void *)cons1);

	pthread_create(
			&cons_th2, &attr, cons_fn, (void *)cons2);
	pthread_create(
			&cons_th3, &attr, cons_fn, (void *)cons3);

	pthread_join(prod_th1, 0);
	pthread_join(prod_th2, 0);
	pthread_join(prod_th3, 0);
	pthread_join(cons_th1, 0);
	pthread_join(cons_th2, 0);
	pthread_join(cons_th3, 0);

	printf("Program Finished\n");
	pthread_exit(0);
	return 0;
}
