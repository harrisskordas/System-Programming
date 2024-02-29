
#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdio.h>


void setup_hooks();

void setup_queue(int pool_size);
void setup_undo_queue();
void setup_socket(int port);
void setup_close();
void save_to_file(FILE * fp);

void worker_execute_service_fd(int fd, FILE * fpLog, FILE * fpStats);

void kernel_execute_accept();

void logical_execute(char * command);

#endif /* EXECUTE_H */

