
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include "sema.h"

sema_t sema_get_new_semaphore()
{
    sema_t *new_sema = malloc(sizeof(sema_t));
    return *new_sema;
}

void sema_init(sema_t *sema, int count)
{
    sema->permit_counter = count;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cv, NULL);
}

void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);

    sema->permit_counter -= 1;
    if (sema->permit_counter < 0)
    {
        pthread_cond_wait(&sema->cv, &sema->mutex);
    }
    pthread_mutex_unlock(&sema->mutex);
}

void sema_post(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    sema->permit_counter += 1;
    if (sema->permit_counter <= 0)
    {
        pthread_cond_signal(&sema->cv);
    }
    pthread_mutex_unlock(&sema->mutex);
}

void sema_destroy(sema_t *sema)
{
    pthread_mutex_unlock(sema);
    pthread_cond_destroy(&sema->cv);
    pthread_mutex_destroy(&sema->mutex);
    free(sema);
}

int sema_getvalue(sema_t *sema)
{
    return sema->permit_counter;
}
