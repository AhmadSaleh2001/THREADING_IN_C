#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Callback function to be executed by the thread
void *print_who_iam_i(void *arg)
{
  int th_id = *(int *)arg;
  free(arg);
  printf("iam: %d\n", th_id);
}

// Function that creates a thread and passes a char* to it
void create_thread_and_print(int th_id)
{
  if (th_id <= 0)
    return;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t thread;
  int *thread_id = calloc(1, sizeof(int));
  *thread_id = th_id;
  // Create a new thread, passing the string as an argument
  if (pthread_create(&thread, NULL, print_who_iam_i, (void *)thread_id) != 0)
  {
    printf("Failed to create thread %d\n", th_id);
    return;
  }
  void *result;
  pthread_join(thread, &result);
  create_thread_and_print(th_id - 1);
}

int main()
{

  int n = 5;
  create_thread_and_print(n);
  return 0;
}
