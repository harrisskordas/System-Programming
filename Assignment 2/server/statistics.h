
#ifndef STATISTICS_H
#define STATISTICS_H

void statistics_add(char * party);

int statistics_find(char * party);

char * statistics_remove(char * party);

void statistics_print(FILE * fp);


#endif /* HISTORY_H */

