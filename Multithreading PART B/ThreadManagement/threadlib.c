
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "threadlib.h"
#include "bitsop.h"

thread_t *
thread_create(thread_t *thread, char *name)
{

    if (!thread)
    {
        thread = calloc(1, sizeof(thread_t));
    }

    strncpy(thread->name, name, sizeof(thread->name));
    thread->thread_created = false;
    thread->arg = NULL;
    thread->thread_fn = NULL;
    pthread_attr_init(&thread->attributes);
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
    // thread->thread_pause_fn(thread->pause_arg);
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
