#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rw_lock.h"

void rw_lock_init(rw_lock_t *rw_lock, int max_writer_threads, int max_reader_threads)
{

  pthread_mutex_init(&rw_lock->state_mutex, NULL);
  pthread_cond_init(&rw_lock->readers_cv, NULL);
  pthread_cond_init(&rw_lock->writers_cv, NULL);

  rw_lock->maximum_number_of_reader_threads = max_reader_threads;
  rw_lock->maximum_number_of_writer_threads = max_writer_threads;
  rw_lock->number_reader_thread_active = 0;
  rw_lock->number_writer_thread_active = 0;
  rw_lock->readers_waiting_threads = 0;
  rw_lock->writers_waiting_threads = 0;
}

void rw_lock_rd_lock(rw_lock_t *rw_lock)
{

  pthread_mutex_lock(&rw_lock->state_mutex);

  rw_lock->readers_waiting_threads++;
  while (rw_lock->number_writer_thread_active > 0 || rw_lock->number_reader_thread_active == rw_lock->maximum_number_of_reader_threads)
  {
    pthread_cond_wait(&rw_lock->readers_cv, &rw_lock->state_mutex);
  }

  assert(rw_lock->number_writer_thread_active == 0);

  rw_lock->readers_waiting_threads--;
  rw_lock->number_reader_thread_active++;
  pthread_mutex_unlock(&rw_lock->state_mutex);
}

void rw_lock_wr_lock(rw_lock_t *rw_lock)
{
  pthread_mutex_lock(&rw_lock->state_mutex);

  rw_lock->writers_waiting_threads++;
  while (rw_lock->number_reader_thread_active > 0 || rw_lock->number_writer_thread_active == rw_lock->maximum_number_of_writer_threads)
  {
    pthread_cond_wait(&rw_lock->writers_cv, &rw_lock->state_mutex);
  }

  assert(rw_lock->number_reader_thread_active == 0);

  rw_lock->writers_waiting_threads--;
  rw_lock->number_writer_thread_active++;

  pthread_mutex_unlock(&rw_lock->state_mutex);
}

void rw_lock_unlock(rw_lock_t *rw_lock)
{
  pthread_mutex_lock(&rw_lock->state_mutex);
  if (rw_lock->number_reader_thread_active > 0) // meaning, current one is reader thread
  {
    rw_lock->number_reader_thread_active--;
    if (rw_lock->number_reader_thread_active == 0)
    {
      if (rw_lock->writers_waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->writers_cv);
      }
      else if (rw_lock->readers_waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->readers_cv);
      }
    }
    else
    {
      if (rw_lock->readers_waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->readers_cv);
      }
    }
  }
  else
  {
    rw_lock->number_writer_thread_active--;
    if (rw_lock->number_writer_thread_active == 0)
    {
      if (rw_lock->readers_waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->readers_cv);
      }
      else if (rw_lock->writers_waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->writers_cv);
      }
    }
    else
    {
      if (rw_lock->writers_waiting_threads > 0)
      {
        pthread_cond_broadcast(&rw_lock->writers_cv);
      }
    }
  }

  pthread_mutex_unlock(&rw_lock->state_mutex);
}

void rw_lock_destroy(rw_lock_t *rw_lock)
{

  pthread_mutex_destroy(&rw_lock->state_mutex);
  pthread_cond_destroy(&rw_lock->readers_cv);
  pthread_cond_destroy(&rw_lock->writers_cv);
  free(rw_lock);
}