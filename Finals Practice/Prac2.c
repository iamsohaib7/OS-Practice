#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  int* array;
  int start;
  int end;
  int key;
  int threadId;
} threadArgs;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* searchKey(void* args) {
  threadArgs* arg = (threadArgs*)args;
  for (int i = arg->start; i < arg->end; i++) {
    if (arg->array[i] == arg->key) {
      pthread_mutex_lock(&mutex);
      int* result = malloc(sizeof(int));
      *result = i;
      pthread_mutex_unlock(&mutex);
      return result;
    }
  }
  return NULL;
}
int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Input Formate is <thread Number> <key>..\n");
    exit(EXIT_FAILURE);
  }
  srand(time(NULL));

  const int size = 1000;
  int array[size];
  for (int i = 0; i < size; i++) {
    array[i] = 1 + rand() % 1000;
  }
  int key = atoi(argv[2]);
  int threadsCnt = atoi(argv[1]);
  int boundrySize = size / threadsCnt;
  int remaining = size % threadsCnt;
  int start = 0;
  threadArgs* args = malloc((size_t)(threadsCnt) * sizeof(threadArgs));
  pthread_t* threads = malloc((size_t)threadsCnt * sizeof(pthread_t));

  for (int i = 0; i < threadsCnt; i++) {
    args[i].array = array;
    args[i].start = start;
    args[i].end = start + boundrySize - 1;
    args[i].key = key;
    args[i].threadId = i;
    if (remaining > 0) {
      args[i].end++;
      remaining--;
    }
    start = args[i].end + 1;
    if (pthread_create(&threads[i], NULL, searchKey, &args[i]) != 0) {
      perror("Failed to create thread");
      exit(EXIT_FAILURE);
    }
  }
  for (int i = 0; i < threadsCnt; i++) {
    void* result;
    pthread_join(threads[i], &result);

    if (result != NULL) {
      printf(
          "Key %d found at index %d in array by thread %d, value in array:  "
          "%d\n",
          key, *((int*)result), i, array[*((int*)result)]);
      free(result);
    }
  }
  free(args);
  free(threads);
  return 0;
}