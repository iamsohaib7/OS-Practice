#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    long long start;
    long long end;
    long long result;
} ThreadArgs;
void *factorialPart(void *args)
{
    ThreadArgs *arg = (ThreadArgs *)args;
    arg->result = 1;
    for (long long i = arg->start; i <= arg->end; i++)
    {
        arg->result *= i;
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <number> <thread_count>\n", argv[0]);
        return 1;
    }

    long long number = atoll(argv[1]);
    int threadCount = atoi(argv[2]);

    pthread_t threads[threadCount];
    ThreadArgs args[threadCount];
    long long blockSize = number / threadCount;
    long long remaining = number % threadCount;

    long long start = 1;
    for (int i = 0; i < threadCount; i++)
    {
        args[i].start = start;
        args[i].end = start + blockSize - 1;
        if (remaining > 0)
        {
            args[i].end++;
            remaining--;
        }
        start = args[i].end + 1;

        if (pthread_create(&threads[i], NULL, factorialPart, &args[i]) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
    }

    long long totalResult = 1;
    for (int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
        totalResult *= args[i].result;
    }

    printf("Factorial of %lld is %lld\n", number, totalResult);
    return 0;
}
