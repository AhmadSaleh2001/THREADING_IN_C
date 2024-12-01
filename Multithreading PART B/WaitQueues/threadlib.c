

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "bitsop.h"
#include "threadlib.h"
#include "gluethread/glthread.h"

thread_t *
thread_create(thread_t *thread, char *name)
{

    if (thread == NULL)
    {
        thread = calloc(1, sizeof(thread_t));
    }

    strncpy(thread->name, name, sizeof(thread->name));
    thread->thread_created = false;
    thread->arg = NULL;
    thread->thread_fn = NULL;
    thread->pause_arg = NULL;
    thread->thread_pause_fn = NULL;
    thread->flags = 0;
    pthread_mutex_init(&thread->mutex_state, NULL);
    pthread_cond_init(&thread->cv, NULL);
    pthread_attr_init(&thread->attributes);
    init_glthread(&thread->wait_glue);
    return thread;
}

void thread_run(thread_t *thread, void *(*thread_fn)(void *), void *arg)
{

    thread->thread_fn = thread_fn;
    thread->arg = arg;
    thread->thread_created = true;
    pthread_create(&thread->thread,
                   &thread->attributes,
                   thread_fn,
                   arg);
}

void thread_set_thread_attribute_joinable_or_detached(
    thread_t *thread, bool joinable)
{

    pthread_attr_setdetachstate(&thread->attributes,
                                joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);
}
void thread_set_pause_fn(thread_t *thread, void *(*thread_pause_fn)(void *), void *pause_arg)
{
    thread->thread_pause_fn = thread_pause_fn;
    thread->pause_arg = pause_arg;
}

void thread_pause(thread_t *thread)
{
    pthread_mutex_lock(&thread->mutex_state);
    while (IS_BIT_SET(thread->flags, THREAD_IS_RUNNING))
    {
        SET_BIT(thread->flags, THREAD_IS_MARKED_FOR_PAUSE);
    }
    pthread_mutex_unlock(&thread->mutex_state);
}

void thread_resume(thread_t *thread)
{
    pthread_mutex_lock(&thread->mutex_state);
    while (IS_BIT_SET(thread->flags, THREAD_IS_PAUSED))
    {
        pthread_cond_signal(&thread->cv);
    }
    pthread_mutex_unlock(&thread->mutex_state);
}

void thread_test_and_pause(thread_t *thread)
{
    pthread_mutex_lock(&thread->mutex_state);
    while (IS_BIT_SET(thread->flags, THREAD_IS_MARKED_FOR_PAUSE))
    {
        UNSET_BIT(thread->flags, THREAD_IS_MARKED_FOR_PAUSE);
        UNSET_BIT(thread->flags, THREAD_IS_RUNNING);
        SET_BIT(thread->flags, THREAD_IS_PAUSED);

        pthread_cond_wait(&thread->mutex_state, &thread->cv);
        thread->thread_pause_fn(thread->pause_arg);
        UNSET_BIT(thread->flags, THREAD_IS_PAUSED);
        SET_BIT(thread->flags, THREAD_IS_RUNNING);
    }

    pthread_mutex_unlock(&thread->mutex_state);
}

void thread_pool_init(thread_pool_t *th_pool)
{

    init_glthread(&th_pool->pool_head);
    pthread_mutex_init(&th_pool->mutex, NULL);
}

void thread_pool_insert_new_thread(thread_pool_t *th_pool, thread_t *thread)
{

    pthread_mutex_lock(&th_pool->mutex);

    assert(IS_GLTHREAD_LIST_EMPTY(&thread->wait_glue));

    glthread_add_last(&th_pool->pool_head, &thread->wait_glue);

    pthread_mutex_unlock(&th_pool->mutex);
}

thread_t *
thread_pool_get_thread(thread_pool_t *th_pool)
{

    thread_t *thread = NULL;
    glthread_t *glthread = NULL;

    pthread_mutex_lock(&th_pool->mutex);
    glthread = dequeue_glthread_first(&th_pool->pool_head);
    if (!glthread)
    {
        pthread_mutex_unlock(&th_pool->mutex);
        return NULL;
    }
    thread = wait_glue_to_thread(glthread);
    pthread_mutex_unlock(&th_pool->mutex);
    return thread;
}

void thread_pool_dispatch_thread(thread_pool_t *th_pool,
                                 void *(*thread_fn)(void *),
                                 void *arg, bool block_caller)
{
    thread_t *thread = thread_pool_get_thread(th_pool);
    if (!thread)
        return;

    thread_execution_data_t *ex;
    if (thread->arg == NULL)
    {
        printf("using new\n");
        ex = calloc(1, sizeof(thread_execution_data_t));
    }
    else
    {
        printf("using old\n");
        ex = thread->arg;
    }
    ex->running_stage_fn = thread_fn;
    ex->running_stage_arg = arg;

    ex->finish_stage_fn = finish_stage_fn;
    ex->thread_pool = th_pool;
    ex->thread = thread;

    thread->thread_fn = execute_then_finish;
    thread->arg = ex;
    printf("thread: %p\n", thread);
    printf("thread name: %s\n", ex->thread->name);

    // printf("*************test********\n");
    // ex->running_stage_fn(ex->running_stage_arg);
    // printf("*************test********\n");

    thread_pool_run_thread(thread);
    if (block_caller)
    {
        ex->thread->semaphore = calloc(1, sizeof(sem_t));
        sem_init(&ex->thread->semaphore, 0, 0);
        sem_wait(&ex->thread->semaphore);
    }
}

void thread_pool_run_thread(thread_t *thread)
{

    if (thread->thread_created)
    {
        pthread_cond_signal(&thread->cv);
    }
    else
    {
        thread->thread_created = true;
        thread_run(thread, thread->thread_fn, thread->arg);
    }
}

void finish_stage_fn(thread_pool_t *thread_pool, thread_t *thread)
{

    pthread_mutex_lock(&thread->mutex_state);

    thread_pool_insert_new_thread(thread_pool, thread);
    if (thread->semaphore)
        sem_post(&thread->semaphore);

    pthread_cond_wait(&thread->cv, &thread->mutex_state);
    pthread_mutex_unlock(&thread->mutex_state);
}

void execute_then_finish(void *arg)
{
    while (1)
    {
        thread_execution_data_t *ex = (thread_execution_data_t *)arg;
        ex->running_stage_fn(ex->running_stage_arg);
        ex->finish_stage_fn(ex->thread_pool, ex->thread);
    }
}

void thread_barrier_print(th_barrier_t *th_barrier)
{
    printf("th_barrier->threshold_count = %u\n", th_barrier->threshold_count);
    printf("th_barrier->curr_wait_count = %u\n", th_barrier->curr_wait_count);
    printf("th_barrier->is_ready_again = %s\n", th_barrier->is_ready_again ? "true" : "false");
}

void thread_barrier_init(th_barrier_t *barrier, uint32_t threshold_count)
{

    barrier->threshold_count = threshold_count;
    barrier->curr_wait_count = 0;
    pthread_cond_init(&barrier->cv, NULL);
    pthread_mutex_init(&barrier->mutex, NULL);
    barrier->is_ready_again = true;
    pthread_cond_init(&barrier->busy_cv, NULL);
}

void thread_barrier_wait(th_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    while (!barrier->is_ready_again)
    {
        pthread_cond_wait(&barrier->busy_cv, &barrier->mutex);
    }
    if (barrier->curr_wait_count + 1 < barrier->threshold_count)
    {
        barrier->curr_wait_count++;
        pthread_cond_wait(&barrier->cv, &barrier->mutex);
        barrier->curr_wait_count--;
        pthread_cond_signal(&barrier->cv);
        pthread_mutex_unlock(&barrier->mutex);
        return;
    }

    barrier->is_ready_again = 0;
    while (barrier->curr_wait_count > 0)
    {
        pthread_cond_signal(&barrier->cv);
        pthread_cond_wait(&barrier->cv, &barrier->mutex);
    }

    barrier->is_ready_again = 1;
    pthread_cond_broadcast(&barrier->busy_cv);
    pthread_mutex_unlock(&barrier->mutex);
}

void thread_barrier_destroy(th_barrier_t *barrier)
{
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cv);
    pthread_cond_destroy(&barrier->busy_cv);
    free(barrier);
}

void wait_queue_init(wait_queue_t *wq, bool is_fifo)
{
    wq->app_mutex = NULL;
    pthread_cond_init(&wq->cv, NULL);
    wq->thread_wait_count = 0;
}

thread_t *
wait_queue_test_and_wait(wait_queue_t *wq,
                         wait_queue_function wait_queue_block_fn_cb,
                         void *arg)
{
    while (wait_queue_block_fn_cb(arg, &wq->app_mutex))
    {
        wq->thread_wait_count++;
        pthread_cond_wait(&wq->cv, wq->app_mutex);
        wq->thread_wait_count--;
    }
    return NULL;
}

void wait_queue_signal(wait_queue_t *wq, bool lock_mutex)
{
    if (!lock_mutex)
        return;
    pthread_mutex_lock(&wq->app_mutex);
    if (wq->thread_wait_count > 0)
        pthread_cond_signal(&wq->cv);
    pthread_mutex_unlock(&wq->app_mutex);
}

void wait_queue_broadcast(wait_queue_t *wq, bool lock_mutex)
{
    if (!lock_mutex || wq->thread_wait_count == 0)
        return;
    pthread_mutex_lock(wq->app_mutex);
    pthread_cond_broadcast(&wq->cv);
    pthread_mutex_unlock(wq->app_mutex);
}

void wait_queue_destroy(wait_queue_t *wq)
{
    pthread_mutex_destroy(wq->app_mutex);
    pthread_cond_destroy(&wq->cv);
    free(wq);
}