
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "execute.h"
#include "tools.h"

#include <stdio.h>
#include <sys/types.h>      /* sockets */
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>          /* gethostbyaddr */
#include <stdlib.h>          /* exit */
#include <string.h>          /* strlen */

#define MAXPIPES 512

void catchinterrupt(int signo) {
    exit(1);
}

void setup_hooks() {
    static struct sigaction act = {0};
    act.sa_handler = catchinterrupt;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
}

void * thread_worker(void * map) {
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;
    int err;

    void ** argv = (void**) map;
    const char * localhost = argv[0];
    int port = *(int*) argv[1];
    char * command = argv[2];
    int fd;

    if ((err = fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        exit(fprintf(stderr, "*** socket failed with exit code: %d ***\n", err));
    }

    if ((rem = gethostbyname(localhost)) == NULL) {
        exit(fprintf(stderr, "*** gethostbyname failed with exit code: %d ***\n", err));
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port); /* Server port */

    if (connect(fd, serverptr, sizeof (server)) < 0) {
        exit(fprintf(stderr, "*** connect failed : %d ***\n", err));
    }

    printf("Worker thread: Sending to %s port %d: %s\n", localhost, port, command);


    const char * helo = "SEND NAME PLEASE";
    const char * error = "ALREADY VOTED";
    const char * question = "SEND VOTE PLEASE";
    const char * verify = "VOTE RECORDED";

    char * buffer = NULL;

    read_all(fd, &buffer);

    if (strcmp(buffer, helo) != 0) {
        exit(fprintf(stderr, "*** I/O error: no match for %s, expeted: %s ***\n", buffer, helo));
    }

    char ** tokens = tokenize(command, " ");
    char * firstname = tokens[0];
    char * lastname = tokens[1];
    char * party = tokens[2];

    write_all(fd, firstname, strlen(firstname));
    write_all(fd, lastname, strlen(lastname));

    free(buffer);

    read_all(fd, &buffer);


    if (strcmp(buffer, error) == 0) {
        printf("Worker thread: Response of  %s:%d: %s => %s \n", localhost, port, command, buffer);
    } else if (strcmp(buffer, question) == 0) {
        write_all(fd, party, strlen(party));
        
        free(buffer);
        
        read_all(fd, &buffer);
        
        printf("Worker thread: Response of  %s:%d: %s => %s \n", localhost, port, command, buffer);
    }


    
    free(buffer);
    
    close(fd);

    free(command);

    free((int*) argv[1]);

    free(tokens);
    
    free(map);

    return NULL;
}

pthread_t logical_execute(const char * localhost, int port, char * command) {

    pthread_t tid;
    int err;

    printf("logical execute: %s:%d => %s \n", localhost, port, command);

    char * data = strdup(command);
    int * hport = malloc(sizeof (int));
    *hport = port;

    void ** map = malloc(sizeof (void*)*3);
    map[0] = (void*) localhost;
    map[1] = (void*) hport;
    map[2] = (void*) data;

    if ((err = pthread_create(&tid, NULL, thread_worker, map)) < 0) { /* New thread */
        return fprintf(stderr, "*** pthread_create failed with exit code: %d ***\n", err);
    }

    return tid;

    //    if (strchr(command, ';') != NULL) {
    //        char ** map = tokenize(command, ";");
    //
    //        dumpTokens("Multiple inline commands given: ", map);
    //
    //        char ** tmp = map;
    //
    //        while (*tmp != NULL) {
    //            logical_execute(*tmp);
    //            tmp++;
    //        }
    //        return;
    //    }
    //
    //    if (strcmp(command, "history") == 0) {
    //        history_print();
    //        return;
    //    }
    //
    //    if (strcmp(command, "aliases") == 0) {
    //        alias_print();
    //        return;
    //    }
    //
    //    if (strcmp(command, "exit") == 0) {
    //        return;
    //    }
    //
    //    if (strcmp(command, "fg") == 0) {
    //        if (childpid_stopped != -1) {
    //            kill(childpid_stopped, SIGCONT);
    //            childpid_stopped = -1;
    //            childpid = -1;
    //        } else {
    //            printf("No child is stopped \n");
    //        }
    //        return;
    //    }
    //
    //    if (strncmp(command, "cd", 2) == 0) {
    //        chdir(command + 3);
    //        return;
    //    }
    //
    //    if (strncmp(command, "createalias", 11) == 0) {
    //        char * cmd = strtok(command, " ");
    //        char * alias = strtok(NULL, " ");
    //        char * rest = strtok(NULL, "\0");
    //
    //        if (rest != NULL && alias != NULL) {
    //            if (*rest == '"') {
    //                rest++;
    //            }
    //            unquote(rest);
    //
    //            clean(alias);
    //            clean(rest);
    //            alias_add(alias, rest);
    //        } else {
    //            printf("createalias failed \n");
    //        }
    //        return;
    //    }
    //
    //    if (strncmp(command, "destroyalias", 12) == 0) {
    //        char * cmd = strtok(command, " ");
    //        char * alias = strtok(NULL, " ");
    //        if (alias != NULL) {
    //            clean(alias);
    //            alias_remove(alias);
    //        } else {
    //            printf("destroyalias failed \n");
    //        }
    //        return;
    //    }
    //
    //    char * testalias = alias_find(command);
    //    if (testalias != NULL) {
    //        printf("%s is an alias \n", command);
    //        logical_execute(testalias);
    //        free(testalias);
    //        return;
    //    }
    //
    //    if (command[0] == '!') {
    //        command = history_find(atoi(command + 1));
    //        if (command) {
    //            history_add(command);
    //            logical_execute(command);
    //            free(command);
    //        }
    //        return;
    //    }
    //
    //    if (strchr(command, '|') == NULL) {
    //        // no pipes, just execute with redirection
    //        if (fork() == 0) {
    //            kernel_execute(command, -1, -1);
    //            exit(0);
    //        } else {
    //            int status = 0;
    //            waitpid(childpid, &status, WSTOPPED | WUNTRACED);
    //
    //            if (!WIFSTOPPED(status)) {
    //                childpid = -1;
    //            }
    //        }
    //    } else {
    //        int pipes = countPipes(command);
    //        int processes = pipes + 1;
    //        char ** pipecommands = tokenize(command, "|");
    //
    //        printf("Total pipes detected: %d \n", pipes);
    //
    //        dumpTokens("Commands with pipes", pipecommands);
    //
    //        for (int i = 0; i < pipes; i++) {
    //            pipe(pipes_descriptors[i]);
    //        }
    //
    //        pid_t * pids = malloc(sizeof (pid_t)*(processes));
    //
    //        for (int i = 0; i < processes; i++) {
    //            if (i == 0) {
    //                if ((pids[i] = fork()) == 0) {
    //                    for (int n = 0; n < pipes; n++) {
    //                        for (int m = 0; m < 2; m++) {
    //                            if (n == 0 && m == 1) {
    //                                continue;
    //                            }
    //                            close(pipes_descriptors[n][m]);
    //                        }
    //                    }
    //
    //                    kernel_execute(pipecommands[i], -1, pipes_descriptors[0][1]);
    //                    exit(1);
    //                }
    //            } else if (i == processes - 1) {
    //                for (int n = 0; n < pipes; n++) {
    //                    for (int m = 0; m < 2; m++) {
    //                        if (n == pipes-1 && m == 0) {
    //                            continue;
    //                        }
    //                        close(pipes_descriptors[n][m]);
    //                    }
    //                }
    //
    //                if ((pids[i] = fork()) == 0) {
    //                    kernel_execute(pipecommands[i], pipes_descriptors[pipes-1][0], -1);
    //                    exit(1);
    //                }
    //            } else {
    //                if ((pids[i] = fork()) == 0) {
    //                    for (int n = 0; n < pipes; n++) {
    //                        for (int m = 0; m < 2; m++) {
    //                            if (n == i-1 && m == 0) {
    //                                continue;
    //                            }
    //                            if (n == i && m == 1) {
    //                                continue;
    //                            }
    //                            close(pipes_descriptors[n][m]);
    //                        }
    //                    }
    //                    
    //                    kernel_execute(pipecommands[i], pipes_descriptors[i-1][0], pipes_descriptors[i][1]);
    //                    exit(1);
    //                }
    //            }
    //        }
    //
    //        for (int i = 0; i < pipes; i++) {
    //            for (int j = 0; j < 2; j++) {
    //                close(pipes_descriptors[i][j]);
    //            }
    //        }
    //
    //        for (int i = 0; i < processes; i++) {
    //            waitpid(pids[i], NULL, 0);
    //        }
    //
    //        free(pids);
    //    }
}