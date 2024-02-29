

#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>


void clean(char * data);

void unquote(char * data);

char ** tokenize(char * data, char * delim);

void dumpTokens(char * header, char ** tokens);

int countPipes(char * data);

int read_all(int fd, char **buff);

int write_all(int fd, const char *buff, size_t size);

#endif /* TOOLS_H */

