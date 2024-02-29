#ifndef ALIASES_H
#define ALIASES_H


void alias_add(char * alias, char * command);

char * alias_find(char * alias);

char * alias_remove(char * alias);

void alias_print();

#endif /* ALIASES_H */

