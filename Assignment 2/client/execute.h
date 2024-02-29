
#ifndef EXECUTE_H
#define EXECUTE_H

void setup_hooks();
pthread_t logical_execute(const char * localhost, int port, char * command);


#endif /* EXECUTE_H */

