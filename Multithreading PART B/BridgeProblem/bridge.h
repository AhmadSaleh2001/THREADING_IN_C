#pragma once

#include <pthread.h>
#include "rw_lock.h"

typedef struct bridge_
{
  pthread_mutex_t state_mutex;
  int n_LR;
  int n_RL;
  rw_lock_t m;
} bridge_t;

void init_bridge(bridge_t *b, int n_LR, int n_RL);
void bridge_enter_lr(bridge_t *b);
void bridge_leave_lr(bridge_t *b);
void bridge_enter_rl(bridge_t *b);
void bridge_leave_rl(bridge_t *b);
void destroy_bridge(bridge_t *b);