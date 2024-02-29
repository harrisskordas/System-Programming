#ifndef VOTES_H
#define VOTES_H


void votes_add(char * alias, char * command);

char * votes_find(char * alias);

char * votes_remove(char * alias);

void votes_print();

#endif /* ALIASES_H */

