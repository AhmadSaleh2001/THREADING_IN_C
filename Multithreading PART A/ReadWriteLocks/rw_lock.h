#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct rw_lock
{
  int is_writer_thread_active;
  int is_reader_thread_active;
  int n;
  pthread_mutex_t state_mutex;

  // to notify waiting threads
  pthread_cond_t cv;
  int waiting_threads;

  pthread_t writer_thread;

} rw_lock_t;

void rw_lock_init(rw_lock_t *rw_lock);

void rw_lock_rd_lock(rw_lock_t *rw_lock);

void rw_lock_wr_lock(rw_lock_t *rw_lock);

void rw_lock_unlock(rw_lock_t *rw_lock);

void rw_lock_destroy(rw_lock_t *rw_lock);