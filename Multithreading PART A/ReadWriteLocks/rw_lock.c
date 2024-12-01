#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rw_lock.h"

void rw_lock_init(rw_lock_t *rw_lock)
{
  rw_lock->is_reader_thread_active = 0;
  rw_lock->writer_thread = 0;
  pthread_mutex_init(&rw_lock->state_mutex, NULL);
  pthread_cond_init(&rw_lock->cv, NULL);

  rw_lock->waiting_threads = 0;
  rw_lock->writer_thread = NULL;
}

void rw_lock_rd_lock(rw_lock_t *rw_lock)
{

  pthread_mutex_lock(&rw_lock->state_mutex);

  rw_lock->waiting_threads++;
  while (rw_lock->is_writer_thread_active)
  {
    pthread_cond_wait(&rw_lock->cv, &rw_lock->state_mutex);
  }

  assert(rw_lock->is_writer_thread_active == 0);
  assert(rw_lock->writer_thread == NULL);

  rw_lock->waiting_threads--;
  rw_lock->is_reader_thread_active = 1;
  rw_lock->n++;
  pthread_mutex_unlock(&rw_lock->state_mutex);
}

void rw_lock_wr_lock(rw_lock_t *rw_lock)
{
  pthread_mutex_lock(&rw_lock->state_mutex);
  rw_lock->waiting_threads++;
  while (rw_lock->is_writer_thread_active || rw_lock->is_reader_thread_active)
  {
    pthread_cond_wait(&rw_lock->cv, &rw_lock->state_mutex);
  }
  assert(rw_lock->is_writer_thread_active == 0);
  assert(rw_lock->is_reader_thread_active == 0);
  assert(rw_lock->writer_thread == NULL);

  rw_lock->writer_thread = pthread_self();
  rw_lock->waiting_threads--;
  rw_lock->is_writer_thread_active = 1;
  rw_lock->n++;
  pthread_mutex_unlock(&rw_lock->state_mutex);
}

void rw_lock_unlock(rw_lock_t *rw_lock)
{
  pthread_mutex_lock(&rw_lock->state_mutex);
  if (rw_lock->writer_thread == NULL) // meaning, current one is reader thread
  {
    rw_lock->n--;
    if (rw_lock->n == 0)
    {
      rw_lock->is_reader_thread_active = 0;
      if (rw_lock->waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->cv);
      }
    }
  }
  else
  {
    assert(rw_lock->writer_thread == pthread_self());
    rw_lock->n--;
    if (rw_lock->n == 0)
    {
      rw_lock->is_writer_thread_active = 0;
      rw_lock->writer_thread = NULL;
      if (rw_lock->waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->cv);
      }
    }
  }

  pthread_mutex_unlock(&rw_lock->state_mutex);
}

void rw_lock_destroy(rw_lock_t *rw_lock)
{

  pthread_mutex_destroy(&rw_lock->state_mutex);
  pthread_cond_destroy(&rw_lock->cv);
  rw_lock->writer_thread = NULL;

  free(rw_lock);
}