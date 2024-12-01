#pragma once

#include <pthread.h>
#include "rw_lock.h"
#include "bridge.h"

void init_bridge(bridge_t *b, int n_LR, int n_RL)
{
  pthread_mutex_init(&b->state_mutex, NULL);
  b->n_LR = n_LR;
  b->n_RL = n_RL;
  rw_lock_init(&b->m, n_LR, n_RL);
}

void bridge_enter_lr(bridge_t *b)
{
  pthread_mutex_lock(&b->state_mutex);

  rw_lock_rd_lock(&b->m);

  pthread_mutex_unlock(&b->state_mutex);
}

void bridge_leave_lr(bridge_t *b)
{
  pthread_mutex_lock(&b->state_mutex);

  rw_lock_unlock(&b->m);

  pthread_mutex_unlock(&b->state_mutex);
}

void bridge_enter_rl(bridge_t *b)
{
  pthread_mutex_lock(&b->state_mutex);

  rw_lock_wr_lock(&b->m);

  pthread_mutex_unlock(&b->state_mutex);
}

void bridge_leave_rl(bridge_t *b)
{
  pthread_mutex_lock(&b->state_mutex);

  rw_lock_unlock(&b->m);

  pthread_mutex_unlock(&b->state_mutex);
}

void destroy_bridge(bridge_t *b)
{

  pthread_mutex_destroy(&b->state_mutex);
  rw_lock_destroy(&b->m);
}