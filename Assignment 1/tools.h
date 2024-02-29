

#ifndef TOOLS_H
#define TOOLS_H

void clean(char * data);

void unquote(char * data);

char ** tokenize(char * data, char * delim);

void dumpTokens(char * header, char ** tokens);

int countPipes(char * data);
#endif /* TOOLS_H */

