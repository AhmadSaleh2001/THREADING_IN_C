

#ifndef __THREAD_LIB__
#define __THREAD_LIB__

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

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
  /* for supporting resume/puase */

  pthread_attr_t attributes;
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

#endif /* __THREAD_LIB__  */
