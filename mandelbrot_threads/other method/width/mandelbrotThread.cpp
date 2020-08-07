#include <stdio.h>
#include <pthread.h>

#include "CycleTimer.h"

typedef struct
{
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
    double *Threadtime;
} WorkerArgs;

extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

//
// workerThreadStart --
//
// Thread entrypoint.
void *workerThreadStart(void *threadArgs)
{

    WorkerArgs *args = static_cast<WorkerArgs *>(threadArgs);

    double startTime[args->numThreads];
    double endTime[args->numThreads];
    startTime[args->threadId] = CycleTimer::currentSeconds();

    // TODO: Implement worker thread here.
    int range = args->width / args->numThreads;
    int startRow = range * args->threadId;
    //int endRow = range * (args->threadId + 1);
    printf("start time Hello world from thread %d\n", args->threadId);
    //printf("workerThreadStart range = %d \t startRow=%d \t \n ", range, startRow);
    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow, range, args->maxIterations, args->output);

    endTime[args->threadId] = CycleTimer::currentSeconds();
    printf("end time Hello world from thread %d\n", args->threadId);
    double timeperiod = (endTime[args->threadId] - startTime[args->threadId]) * 1000;
    args->Threadtime[args->threadId] = timeperiod;
    printf("Thread %d time is %.3f \n", args->threadId, timeperiod);

    return NULL;
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Multi-threading performed via pthreads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    const static int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    pthread_t workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];
    double threadtime[numThreads];
    for (int j = 0; j < numThreads; j++)
    {
        threadtime[j] = 0.f;
    }
    for (int i = 0; i < numThreads; i++)
    {
        // TODO: Set thread arguments here.
        args[i].threadId = i;
        args[i].numThreads = numThreads;
        args[i].output = output;
        args[i].maxIterations = maxIterations;
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;

        args[i].Threadtime = threadtime;
    }

    // Fire up the worker threads.  Note that numThreads-1 pthreads
    // are created and the main app thread is used as a worker as
    // well.

    for (int i = 1; i < numThreads; i++)
        pthread_create(&workers[i], NULL, workerThreadStart, &args[i]);

    workerThreadStart(&args[0]);

    // wait for worker threads to complete
    for (int i = 1; i < numThreads; i++)
    {
        pthread_join(workers[i], NULL);
    }
    for (int i = 0; i < numThreads; i++)
    {
        printf("thread %d time = %.3f \n", i, threadtime[i]);
    }
}
