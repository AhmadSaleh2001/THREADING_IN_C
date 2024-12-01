#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define MAX_LINES 400000    // Maximum number of lines to read
#define MAX_LINE_LENGTH 256 // Maximum length of each line

// Function prototypes
char **readLinesFromFile(const char *filename, int *lineCount);
void freeLines(char **lines, int lineCount);
int countWords(char *line);
void create_thread_and_print(pthread_t *thread, int start, int end);
void *readYourPart(void *arg);
char **lines;
pthread_mutex_t lock;

int main(int argc, char **argv)
{
  int lineCount = 0;
  lines = readLinesFromFile("example.txt", &lineCount);

  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("Mutex init failed\n");
    return 1;
  }

  // Check if lines were read successfully
  if (lines != NULL)
  {
    // Print the lines read from the file
    clock_t startTime, endTime;

    int count = 0;
    const int NUMBER_OF_THREADS = atoi(argv[1]);
    const int FOR_EACH = MAX_LINES / NUMBER_OF_THREADS;

    pthread_t *threads = malloc(NUMBER_OF_THREADS * sizeof(pthread_t));
    startTime = clock();
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
      int start = i * FOR_EACH;
      int end = (i + 1) * FOR_EACH;
      create_thread_and_print(&threads[i], start, end - 1);
    }

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
      void *thread_result;
      pthread_join(threads[i], &thread_result);
      pthread_mutex_lock(&lock);
      pthread_join(threads[i], &thread_result);
      count += *((int *)thread_result);
      pthread_mutex_unlock(&lock);
    }

    endTime = clock();
    printf("number of words: %d\n", count);

    double timeTaken = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("Time taken to read the file: %.6f seconds\n", timeTaken);

    // Free allocated memory for lines
    freeLines(lines, lineCount);
  }

  return EXIT_SUCCESS;
}

// Function to read lines from a file
char **readLinesFromFile(const char *filename, int *lineCount)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    perror("Error opening file");
    return NULL;
  }

  char **lines = malloc(MAX_LINES * sizeof(char *)); // Array to hold pointers to lines
  if (lines == NULL)
  {
    perror("Memory allocation error");
    fclose(file);
    return NULL;
  }

  char buffer[MAX_LINE_LENGTH]; // Buffer for reading lines
  while (*lineCount < MAX_LINES && fgets(buffer, sizeof(buffer), file))
  {
    // Allocate memory for the line and copy it from the buffer
    lines[*lineCount] = malloc(strlen(buffer) + 1); // +1 for the null terminator
    if (lines[*lineCount] == NULL)
    {
      perror("Memory allocation error");
      freeLines(lines, *lineCount); // Free already allocated lines
      fclose(file);
      return NULL;
    }
    strcpy(lines[*lineCount], buffer); // Copy the buffer to the allocated memory
    (*lineCount)++;                    // Increment line count
  }

  fclose(file);
  return lines;
}

// Function to free allocated memory for lines
void freeLines(char **lines, int lineCount)
{
  for (int i = 0; i < lineCount; i++)
  {
    free(lines[i]); // Free each allocated line
  }
  free(lines); // Free the array of pointers
}

int countWords(char *line)
{
  int ans = 1;
  for (int i = 0; line[i] != '\0'; i++)
  {
    if (line[i] == ' ')
      ans++;
  }

  return ans;
}

void *readYourPart(void *arg)
{
  int *parts = (int *)arg;
  int ans = 0;
  for (int i = parts[0]; i <= parts[1]; i++)
  {
    ans += countWords(lines[i]);
  }
  free(parts);
  int *result = calloc(1, sizeof(int));
  *result = ans;
  return (void *)result;
}

// Function that creates a thread and passes a char* to it
void create_thread_and_print(pthread_t *thread, int start, int end)
{

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  int *parts = calloc(1, 2 * sizeof(int));
  parts[0] = start;
  parts[1] = end;
  // Create a new thread, passing the string as an argument
  if (pthread_create(thread, NULL, readYourPart, (void *)parts) != 0)
  {
    printf("Failed to create thread\n");
    return;
  }
}