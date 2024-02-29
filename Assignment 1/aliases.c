
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aliases.h"

#define ALIAS_TABLE_SIZE 100

static char * alias_map_keys[ALIAS_TABLE_SIZE] = { 0 };
static char * alias_map_commands[ALIAS_TABLE_SIZE] = { 0 };

void alias_add(char * alias, char * command) {
    if (strlen(command) == 0 || strlen(alias) == 0) {
        return;
    }
    
    for (int i=0;i<ALIAS_TABLE_SIZE;i++) {
        if (alias_map_keys[i] == NULL) {
            alias_map_keys[i] = strdup(alias);
            alias_map_commands[i] = strdup(command);
            return;
        }
        
        if (strcmp(alias_map_keys[i], alias) == 0) {
            return;
        }
    }
}

char * alias_find(char * alias) {
    for (int i=0;i<ALIAS_TABLE_SIZE;i++) {
        if (alias_map_keys[i] != NULL && strcmp(alias_map_keys[i], alias) == 0) {
            return strdup(alias_map_commands[i]);
        }
    }
    return NULL;
}

char * alias_remove(char * alias) {
    for (int i=0;i<ALIAS_TABLE_SIZE;i++) {
        if (alias_map_keys[i] != NULL && strcmp(alias_map_keys[i], alias) == 0) {
            free(alias_map_commands[i]);
            free(alias_map_keys[i]);
            alias_map_commands[i] = NULL;
            alias_map_keys[i] = NULL;
            break;
        }
    }
    return NULL;
}

void alias_print() {
    for (int i=0;i<ALIAS_TABLE_SIZE;i++) {
        if (alias_map_keys[i] == NULL) {
            continue;
        }
        
        printf("%-20s %-20s \n", alias_map_keys[i], alias_map_commands[i]);
    }
}