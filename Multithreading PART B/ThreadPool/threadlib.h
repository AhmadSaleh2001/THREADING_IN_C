

#ifndef __THREAD_LIB__
#define __THREAD_LIB__

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <semaphore.h>
#include "gluethread/glthread.h"

#define THREAD_IS_RUNNING (1 << 0)

// when thread marked to pause, but not paused yet
#define THREAD_IS_MARKED_FOR_PAUSE (1 << 1)

#define THREAD_IS_PAUSED (1 << 2)

#define THREAD_IS_BLOCKED (1 << 3)

typedef struct thread_
{

  char name[32];
  bool thread_created;
  pthread_t thread;
  void *arg;
  void *(*thread_fn)(void *);

  /* for supporting resume/puase */
  void *(*thread_pause_fn)(void *);
  void *pause_arg;
  uint32_t flags;
  pthread_mutex_t mutex_state;
  pthread_cond_t cv;
  /* for supporting resume/pause */

  sem_t *semaphore;
  pthread_attr_t attributes;

  /*for thread pool*/
  glthread_t wait_glue;
  /*for thread pool*/

} thread_t;

thread_t *
thread_create(thread_t *thread, char *name);

void thread_run(thread_t *thread, void *(*thread_fn)(void *), void *arg);

void thread_set_thread_attribute_joinable_or_detached(
    thread_t *thread, bool joinable);

/* Thead pausing and resuming */

void thread_set_pause_fn(thread_t *thread,
                         void *(*thread_pause_fn)(void *),
                         void *pause_arg);

void thread_pause(thread_t *thread);

void thread_resume(thread_t *thread);

void thread_test_and_pause(thread_t *thread);

typedef struct thread_pool_
{
  glthread_t pool_head;
  pthread_mutex_t mutex;
} thread_pool_t;

GLTHREAD_TO_STRUCT(wait_glue_to_thread, thread_t, wait_glue)

void thread_pool_init(thread_pool_t *th_pool);

void thread_pool_insert_new_thread(thread_pool_t *th_pool, thread_t *thread);

thread_t *
thread_pool_get_thread(thread_pool_t *th_pool);

void thread_pool_run_thread(thread_t *thread);

void thread_pool_dispatch_thread(thread_pool_t *th_pool,
                                 void *(*thread_fn)(void *),
                                 void *arg, bool block_caller);

typedef struct thread_execution_data_
{
  void *(*running_stage_fn)(void *);
  void *running_stage_arg;

  void *(*finish_stage_fn)(void *, void *);

  thread_pool_t *thread_pool;
  thread_t *thread;

} thread_execution_data_t;

void finish_stage_fn(thread_pool_t *thread_pool, thread_t *thread);
void execute_then_finish(void *arg);

#endif /* __THREAD_LIB__  */
