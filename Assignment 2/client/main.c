
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "tools.h"
#include "execute.h"

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "*** INVALID COMMAND LINE ARGUMENTS ***\n");
        return argc;
    }

    const char * localhost = argv[1];
    int port = atoi(argv[2]);
    const char * filename = argv[3];
    int err = 0;

    fprintf(stdout, "Starting client at localhost for %s:%d. \n", localhost, port);
    fprintf(stdout, "Record file: %s \n", filename);

    size_t bufsize = 10000;
    size_t characters = 0;
    char *command_buffer = (char *) malloc(bufsize * sizeof (char));

    if (command_buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }

    setup_hooks();

    FILE * fp = fopen(filename, "r");

    if (!fp) {
        exit(fprintf(stderr, "*** fopen(filename, \"r\") failed with exit code: NULL ***\n"));
    }

    int numWorkerThreads = 0;

    do {
        strcpy(command_buffer, "");

        characters = getline(&command_buffer, &bufsize, fp);

        if (characters > 0 && strlen(command_buffer) > 0) {
            numWorkerThreads++;
        }
    } while (characters > 0 && !feof(fp));

    rewind(fp);

    fprintf(stdout, "Total thread pool: %d ... \n", numWorkerThreads);

    pthread_t thread_ids[numWorkerThreads];

    int lineno = 0;

    do {
        strcpy(command_buffer, "");

        characters = getline(&command_buffer, &bufsize, fp);

        if (characters > 0 && strlen(command_buffer) > 0) {
            clean(command_buffer);

            printf("Vote command: '%s' \n", command_buffer);
           
            thread_ids[lineno++] =logical_execute(localhost, port, command_buffer);
        }
        
//        break;
    } while (characters > 0 && !feof(fp));

    free(command_buffer);

    fclose(fp);

    for (int i = 0; i < lineno; i++) {
        fprintf(stdout, "waiting for thread %d to finish ... \n", i);

        if ((err = pthread_join(thread_ids[i], NULL)) < 0) { /* New thread */
            return fprintf(stderr, "*** pthread_join failed with exit code: %d ***\n", err);
        } else {
            fprintf(stdout, "pthread_join successded for thread: %d ***\n", i);
        }
    }

    return 0;
}

