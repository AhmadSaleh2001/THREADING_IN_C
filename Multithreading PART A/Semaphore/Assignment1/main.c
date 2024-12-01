#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

pthread_t t1, t2;
sem_t sem1, sem2;

void goAHead1(void *arg)
{
  sem_wait(&sem1);
  printf("%d", 1);
  sem_post(&sem2);
}

void goAHead2(void *args)
{
  printf("%d", 2);
  sem_post(&sem1);
  sem_wait(&sem2);
}

int main()
{

  sem_init(&sem1, 0, 0);
  sem_init(&sem2, 0, 0);

  pthread_create(&t1, NULL, goAHead1, NULL);
  pthread_create(&t2, NULL, goAHead2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("\nFinished Execution\n");
  return 0;
}