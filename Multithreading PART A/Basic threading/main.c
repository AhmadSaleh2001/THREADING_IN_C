#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Callback function to be executed by the thread
void *print_string_cb(void *arg)
{
  char *message = (char *)arg;
  int cnt = 0;
  while (cnt++ < 10)
  {
    printf("%s\n", message);
    sleep(1); // Wait for 1 second
  }
  return NULL;
}

// Function that creates a thread and passes a char* to it
void create_thread_and_print(const char *str)
{
  pthread_t thread;
  // Create a new thread, passing the string as an argument
  if (pthread_create(&thread, NULL, print_string_cb, (void *)str) != 0)
  {
    perror("Failed to create thread");
    return;
  }
  // Optionally, you can use pthread_join to wait for the thread to finish
  // but in this case, the thread runs indefinitely, so we don't use it.
}

int main()
{
  const char *myMessage = "Hello, world!";
  create_thread_and_print(myMessage);

  // lets kill current thread (not process)
  // (thread execution the main function, this lead to not reach "return 0"
  // (it will kill the process) line, which will cause the program to be running)
  pthread_exit(0);

  return 0;
}
