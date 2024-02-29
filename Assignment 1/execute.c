
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
#include "execute.h"
#include "history.h"
#include "tools.h"
#include "aliases.h"

#define MAXPIPES 512

static pid_t childpid = -1;
static pid_t childpid_stopped = -1;
static int pipes_descriptors[MAXPIPES][2];

void catchinterrupt(int signo) {
    if (childpid == -1) {
        return;
    }
    fprintf(stderr, "*** SENDING SIGINT TO CHILD ***\n");
    kill(childpid, SIGINT);
}

void catchstop(int signo) {
    if (childpid == -1) {
        return;
    }
    fprintf(stderr, "*** SENDING SIGSTOP TO CHILD ***\n");

    kill(childpid, SIGSTOP);
}

void setup_hooks() {
    {
        static struct sigaction act = {0};
        act.sa_handler = catchinterrupt;
        act.sa_flags = SA_RESTART;
        sigfillset(&(act.sa_mask));
        sigaction(SIGINT, &act, NULL);
    }
    {
        static struct sigaction act = {0};
        act.sa_handler = catchinterrupt;
        act.sa_flags = SA_RESTART;
        sigfillset(&(act.sa_mask));
        sigaction(SIGTSTP, &act, NULL);
    }
}

void kernel_execute(char * command, int fd_in, int fd_out) {
    printf("kernel execute (child process): %s \n", command);

    char ** map = tokenize(command, " ");
    char ** tmp = map;

    while (*tmp != NULL) {
        if (strcmp(*tmp, "<") == 0) {
            *tmp = NULL;
            tmp++;

            fd_in = open(*tmp, O_RDONLY);

            if (fd_in == -1) {
                perror("open O_RDONLY");
                return;
            }
        } else if (strcmp(*tmp, ">") == 0) {
            *tmp = NULL;
            tmp++;

            fd_out = open(*tmp, O_CREAT | O_TRUNC | O_WRONLY, 0666);

            if (fd_out == -1) {
                perror("open O_CREAT | O_TRUNC | O_WRONLY");
                return;
            }
        } else if (strcmp(*tmp, ">>") == 0) {
            *tmp = NULL;
            fd_out = open(*tmp, O_APPEND | O_WRONLY, 0666);

            if (fd_out == -1) {
                perror("open O_CREAT | O_APPEND | O_WRONLY");
                return;
            }
        } else if (strcmp(*tmp, "&") == 0) {
            *tmp = NULL;
            break;
        }
        tmp++;
    }

    if (fd_in != -1) {
        dup2(fd_in, 0);
        close(fd_in);
    }
    if (fd_out != -1) {
        dup2(fd_out, 1);
        close(fd_out);
    }

    if (execvp(map[0], map) < 0) {
        fprintf(stderr, "fork failed \n");
        exit(1);
    } else {
        exit(0);
    }
}

void logical_execute(char * command) {
    printf("logical execute: %s \n", command);

    if (strchr(command, ';') != NULL) {
        char ** map = tokenize(command, ";");

        dumpTokens("Multiple inline commands given: ", map);

        char ** tmp = map;

        while (*tmp != NULL) {
            logical_execute(*tmp);
            tmp++;
        }
        return;
    }

    if (strcmp(command, "history") == 0) {
        history_print();
        return;
    }

    if (strcmp(command, "aliases") == 0) {
        alias_print();
        return;
    }

    if (strcmp(command, "exit") == 0) {
        return;
    }

    if (strcmp(command, "fg") == 0) {
        if (childpid_stopped != -1) {
            kill(childpid_stopped, SIGCONT);
            childpid_stopped = -1;
            childpid = -1;
        } else {
            printf("No child is stopped \n");
        }
        return;
    }

    if (strncmp(command, "cd", 2) == 0) {
        chdir(command + 3);
        return;
    }

    if (strncmp(command, "createalias", 11) == 0) {
        char * cmd = strtok(command, " ");
        char * alias = strtok(NULL, " ");
        char * rest = strtok(NULL, "\0");

        if (rest != NULL && alias != NULL) {
            if (*rest == '"') {
                rest++;
            }
            unquote(rest);

            clean(alias);
            clean(rest);
            alias_add(alias, rest);
        } else {
            printf("createalias failed \n");
        }
        return;
    }

    if (strncmp(command, "destroyalias", 12) == 0) {
        char * cmd = strtok(command, " ");
        char * alias = strtok(NULL, " ");
        if (alias != NULL) {
            clean(alias);
            alias_remove(alias);
        } else {
            printf("destroyalias failed \n");
        }
        return;
    }

    char * testalias = alias_find(command);
    if (testalias != NULL) {
        printf("%s is an alias \n", command);
        logical_execute(testalias);
        free(testalias);
        return;
    }

    if (command[0] == '!') {
        command = history_find(atoi(command + 1));
        if (command) {
            history_add(command);
            logical_execute(command);
            free(command);
        }
        return;
    }

    if (strchr(command, '|') == NULL) {
        // no pipes, just execute with redirection
        if (fork() == 0) {
            kernel_execute(command, -1, -1);
            exit(0);
        } else {
            int status = 0;
            waitpid(childpid, &status, WSTOPPED | WUNTRACED);

            if (!WIFSTOPPED(status)) {
                childpid = -1;
            }
        }
    } else {
        int pipes = countPipes(command);
        int processes = pipes + 1;
        char ** pipecommands = tokenize(command, "|");

        printf("Total pipes detected: %d \n", pipes);

        dumpTokens("Commands with pipes", pipecommands);

        for (int i = 0; i < pipes; i++) {
            pipe(pipes_descriptors[i]);
        }

        pid_t * pids = malloc(sizeof (pid_t)*(processes));

        for (int i = 0; i < processes; i++) {
            if (i == 0) {
                if ((pids[i] = fork()) == 0) {
                    for (int n = 0; n < pipes; n++) {
                        for (int m = 0; m < 2; m++) {
                            if (n == 0 && m == 1) {
                                continue;
                            }
                            close(pipes_descriptors[n][m]);
                        }
                    }

                    kernel_execute(pipecommands[i], -1, pipes_descriptors[0][1]);
                    exit(1);
                }
            } else if (i == processes - 1) {
                for (int n = 0; n < pipes; n++) {
                    for (int m = 0; m < 2; m++) {
                        if (n == pipes-1 && m == 0) {
                            continue;
                        }
                        close(pipes_descriptors[n][m]);
                    }
                }

                if ((pids[i] = fork()) == 0) {
                    kernel_execute(pipecommands[i], pipes_descriptors[pipes-1][0], -1);
                    exit(1);
                }
            } else {
                if ((pids[i] = fork()) == 0) {
                    for (int n = 0; n < pipes; n++) {
                        for (int m = 0; m < 2; m++) {
                            if (n == i-1 && m == 0) {
                                continue;
                            }
                            if (n == i && m == 1) {
                                continue;
                            }
                            close(pipes_descriptors[n][m]);
                        }
                    }
                    
                    kernel_execute(pipecommands[i], pipes_descriptors[i-1][0], pipes_descriptors[i][1]);
                    exit(1);
                }
            }
        }

        for (int i = 0; i < pipes; i++) {
            for (int j = 0; j < 2; j++) {
                close(pipes_descriptors[i][j]);
            }
        }

        for (int i = 0; i < processes; i++) {
            waitpid(pids[i], NULL, 0);
        }

        free(pids);
    }
}