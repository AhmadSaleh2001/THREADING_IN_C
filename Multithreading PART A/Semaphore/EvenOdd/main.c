#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "sema.h"

pthread_t t1, t2;
sema_t sem1, sem2;

void goAHead1(void *arg)
{
  for (int i = 1; i < 15; i += 2)
  {
    printf("%d\n", i);
    sema_post(&sem2);
    sema_wait(&sem1);
  }
}

void goAHead2(void *args)
{
  for (int i = 2; i <= 15; i += 2)
  {
    sema_wait(&sem2);
    printf("%d\n", i);
    sema_post(&sem1);
  }
}

int main()
{

  sem1 = sema_get_new_semaphore();
  sem2 = sema_get_new_semaphore();
  sema_init(&sem1, 0);
  sema_init(&sem2, 0);

  pthread_create(&t1, NULL, goAHead1, NULL);
  pthread_create(&t2, NULL, goAHead2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("\nFinished Execution\n");
  return 0;
}