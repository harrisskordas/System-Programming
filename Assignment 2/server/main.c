
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#include "execute.h"
#include "prod_cons.h"

void * thread_worker(void * map) {
    void ** argv = (void**) map;
    FILE * fpLog = (FILE*) argv[0];
    FILE * fpStats = (FILE*) argv[1];
    
    fprintf(stdout, " * Worker thread started, log: %p, stats: %p . \n", fpLog, fpStats);

    while (true) {
        int fd = pool_obtain(NULL);

        if (fd > 0) {
            worker_execute_service_fd(fd, fpLog, fpStats);
        } else {
            break;
        }
    }

    return 0;
}

void * thread_master(void * map) {
    void ** argv = (void**) map;
    int port = atoi((char*) argv[1]);
    int numWorkerThreads = atoi((char*) argv[2]);
    const char * pollLog = (const char *) argv[3];
    const char * pollStats = (const char *) argv[4];
    pthread_t * thread_ids = (pthread_t*) argv[5];

    int err;

    fprintf(stdout, " * Master thread started at localhost:%d with %d threads. \n", port, numWorkerThreads);
    fprintf(stdout, " * Master thread Log file: %s \n", pollLog);
    fprintf(stdout, " * Master thread Statistics file: %s \n", pollStats);

    setup_socket(port);

    FILE * fpLog = fopen(pollLog, "w+");

    if (!fpLog) {
        fprintf(stderr, "*** fopen(pollLog, \"w+\") failed with exit code: NULL ***\n");
        return NULL;
    }

    FILE * fpStats = fopen(pollStats, "w+");

    if (!fpStats) {
        fprintf(stderr, "*** fopen(fpStats, \"w+\") failed with exit code: NULL ***\n");
        return NULL;
    }
    
    fprintf(stdout, " * Master thread - poll log   fp: %p \n", pollLog);
    fprintf(stdout, " * Master thread - poll stats fp: %p \n", fpStats);
    

    void * mapWorker[2] = {
        fpLog,
        fpStats
    };
    
    
    for (int i = 1; i <= numWorkerThreads; i++) {
        if ((err = pthread_create(&thread_ids[i], NULL, thread_worker, mapWorker)) < 0) { /* New thread */
            fprintf(stderr, "*** pthread_create failed with exit code: %d ***\n", err);
            return NULL;
        }
    }

    kernel_execute_accept();

    for (int i = 1; i <= numWorkerThreads; i++) {
        pool_place(NULL, 0);
    }
    
    save_to_file(fpStats);
    
    fclose(fpLog);
    
    fclose(fpStats);
    
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 6) {
        fprintf(stderr, "*** INVALID COMMAND LINE ARGUMENTS ***\n");
        return argc;
    }

    int port = atoi(argv[1]);
    int numWorkerThreads = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    const char * pollLog = argv[4];
    const char * pollStats = argv[5];
    int err = 0;

    fprintf(stdout, "Starting server at localhost:%d with %d threads and buffer size %d. \n", port, numWorkerThreads, bufferSize);
    fprintf(stdout, "Log file: %s \n", pollLog);
    fprintf(stdout, "Statistics file: %s \n", pollStats);

    pthread_t thread_ids[numWorkerThreads + 1];

    size_t bufsize = 1024;
    size_t characters = 0;
    char *command_buffer = (char *) malloc(bufsize * sizeof (char));

    if (command_buffer == NULL) {
        return fprintf(stderr, "*** Unable to allocate buffer ***\n");
    }

    void *map[] = {
        NULL,
        argv[1],
        argv[2],
        argv[4],
        argv[5],
        &thread_ids
    };

    setup_hooks();

    setup_queue(bufferSize);

    if ((err = pthread_create(&thread_ids[0], NULL, thread_master, map)) < 0) { /* New thread */
        return fprintf(stderr, "*** pthread_create failed with exit code: %d ***\n", err);
    }

    do {
        strcpy(command_buffer, "exit_by_signal");

        printf("server-now: Press ctrl+C to close the server ... > ");

        characters = getline(&command_buffer, &bufsize, stdin);
    } while (characters > 0 && !feof(stdin) && strcmp(command_buffer, "exit") != 0 && strcmp(command_buffer, "exit_by_signal") != 0);

    free(command_buffer);

    setup_close();

    for (int i = 0; i < numWorkerThreads + 1; i++) {
        fprintf(stdout, "waiting for thread %d to finish ... \n", i);

        if ((err = pthread_join(thread_ids[i], NULL)) < 0) { /* New thread */
            return fprintf(stderr, "*** pthread_join failed with exit code: %d ***\n", err);
        } else {
            fprintf(stdout, "pthread_join successded for thread: %d ***\n", i);
        }
    }

    setup_undo_queue();

    setup_close();
    
    return 0;
}

