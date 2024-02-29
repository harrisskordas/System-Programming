
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "execute.h"
#include "statistics.h"
#include "tools.h"
#include <stdio.h>
#include <sys/wait.h>      /* sockets */
#include <sys/types.h>      /* sockets */
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <netdb.h>          /* gethostbyaddr */
#include <unistd.h>          /* fork */  
#include <stdlib.h>          /* exit */
#include <ctype.h>          /* toupper */
#include <signal.h>
#include <pthread.h>          /* signal */

#include "votes.h"
#include "prod_cons.h"

static int welcome_socket_descriptor = 0;

static pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t votes_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

void catchinterrupt(int signo) {
    fclose(stdin);
}

void setup_hooks() {
    static struct sigaction act = {0};
    act.sa_handler = catchinterrupt;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
}

void setup_queue(int pool_size) {
    pool_initialize(NULL, pool_size);

    fprintf(stdout, "queue initialized with size: %d \n", pool_size);
}

void setup_undo_queue() {
    pool_destroy(NULL);
}

void setup_close() {
    shutdown(welcome_socket_descriptor, SHUT_RDWR);
    close(welcome_socket_descriptor);
}

void setup_socket(int port) {
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *) &server;
    int err;

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    if ((err = welcome_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        exit(fprintf(stderr, "*** socket failed with exit code: %d ***\n", err));
    }

    if (bind(welcome_socket_descriptor, serverptr, sizeof (server)) < 0) {
        exit(fprintf(stderr, "*** bind failed with exit code: %d ***\n", err));
    }

    if (listen(welcome_socket_descriptor, 5) < 0) {
        exit(fprintf(stderr, "*** listen failed with exit code: %d ***\n", err));
    }
}

void kernel_execute_accept() {
    struct sockaddr_in client;
    socklen_t clientlen = sizeof (client);
    struct sockaddr *clientptr = (struct sockaddr *) &client;

    while (true) {
        int newsock = accept(welcome_socket_descriptor, clientptr, &clientlen);
        if (newsock > 0) {
            pool_place(NULL, newsock);
        } else {
            break;
        }
    }
}

void worker_execute_service_fd(int fd, FILE * fpLog, FILE * fpStats) {
    const char * helo = "SEND NAME PLEASE";
    const char * error = "ALREADY VOTED";
    const char * question = "SEND VOTE PLEASE";
    const char * verify = "VOTE RECORDED";

    fprintf(stdout, " * Worker thread servicing: %d, log: %p, stats: %p . \n", fd, fpLog, fpStats);

    char * firstname = NULL;
    char * lastname = NULL;

    write_all(fd, helo, strlen(helo));

    read_all(fd, &firstname);
    read_all(fd, &lastname);

    char *name = malloc(sizeof (char)*(strlen(firstname) + strlen(lastname) + 10));

    clean(firstname);
    clean(lastname);

    strcpy(name, firstname);
    strcat(name, " ");
    strcat(name, lastname);

    printf("Name received: %s \n", name);
    
    pthread_mutex_lock(&votes_mutex);
    char * testparty = votes_find(name);
    
    if (testparty != NULL) {
        pthread_mutex_unlock(&votes_mutex);
        
        write_all(fd, error, strlen(error));
    } else {
        write_all(fd, question, strlen(question));
        
        char * party = NULL;
        
        read_all(fd, &party);
        
        votes_add(name, party);
        
        pthread_mutex_unlock(&votes_mutex);
        
        write_all(fd, verify, strlen(verify));
        
        pthread_mutex_lock(&file_mutex);
        fprintf(fpLog, "%s %s \n", name, party);
        pthread_mutex_unlock(&file_mutex);
        
        
        pthread_mutex_lock(&stats_mutex);
        statistics_add(party);        
        pthread_mutex_unlock(&stats_mutex);
        
        free(party);       
    }



    free(firstname);
    free(lastname);
    free(name);
}

void save_to_file(FILE * fpStats) {
    statistics_print(fpStats);
    
}