
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define HISTORY_SIZE 20

static char * history_map[HISTORY_SIZE] = { 0 };
static int pos = 0;

void history_add(char * command) {
    if (strlen(command) == 0 || command[0] == '!') {
        return;
    }
    if (pos > 0 && strcmp(history_map[pos-1], command) == 0) {
        return;
    }
    if (pos == HISTORY_SIZE) {
        free(history_map[0]);
        for (int i=0;i<HISTORY_SIZE-1;i++){ 
            history_map[i] = history_map[i+1];
        }        
        pos = HISTORY_SIZE -1;
        history_map[pos] = NULL;
    }
    
    history_map[pos] = strdup(command);
    pos++;
} 

char * history_find(int n) {
    if (n < HISTORY_SIZE) {
        return strdup(history_map[n]);
    } else {
        return NULL;
    }
}

void history_print() {
    for (int i=0;i<HISTORY_SIZE;i++) {
        if (history_map[i] != NULL) {
            printf(" [%d]: %s \n", i, history_map[i]);
        } else {
            break;
        }
    }
}