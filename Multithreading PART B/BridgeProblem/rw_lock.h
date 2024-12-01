#pragma once

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct rw_lock
{

  int maximum_number_of_writer_threads;
  int maximum_number_of_reader_threads;
  int number_writer_thread_active;
  int number_reader_thread_active;

  pthread_mutex_t state_mutex;

  // to notify waiting threads
  pthread_cond_t readers_cv;
  pthread_cond_t writers_cv;
  int readers_waiting_threads;
  int writers_waiting_threads;

} rw_lock_t;

void rw_lock_init(rw_lock_t *rw_lock, int max_writer_threads, int max_reader_threads);

void rw_lock_rd_lock(rw_lock_t *rw_lock);

void rw_lock_wr_lock(rw_lock_t *rw_lock);

void rw_lock_unlock(rw_lock_t *rw_lock);

void rw_lock_destroy(rw_lock_t *rw_lock);