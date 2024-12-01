#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Callback function to be executed by the thread
void *calc_square(void *arg)
{
  int th_id = *(int *)arg;
  free(arg);
  int cnt = 0;
  while (cnt++ < th_id)
  {
    printf("thread %d doing work\n", th_id);
    sleep(1); // Wait for 1 second
  }

  int *result = calloc(1, sizeof(int));
  *result = th_id * th_id;
  return (void *)result;
}

// Function that creates a thread and passes a char* to it
void create_thread_and_print(pthread_t *thread, int th_id)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  int *thread_id = calloc(1, sizeof(int));
  *thread_id = th_id;
  // Create a new thread, passing the string as an argument
  if (pthread_create(thread, NULL, calc_square, (void *)thread_id) != 0)
  {
    printf("Failed to create thread %d\n", th_id);
    return;
  }
}

int main()
{

  pthread_t thread1, thread2;
  void *thread1_result;
  void *thread2_result;

  int thread_id1 = 2;
  int thread_id2 = 5;

  create_thread_and_print(&thread1, thread_id1);
  create_thread_and_print(&thread2, thread_id2);
  pthread_join(thread1, &thread1_result);

  if (thread1_result)
  {
    printf("answer from thread %d is: %d\n", thread_id1, *((int *)thread1_result));
  }

  pthread_join(thread2, &thread2_result);

  if (thread2_result)
  {
    printf("answer from thread %d is: %d\n", thread_id2, *((int *)thread2_result));
  }

  return 0;
}
