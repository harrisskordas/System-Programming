
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "statistics.h"

#define VOTES_TABLE_SIZE 10000

static char * party_map_keys[VOTES_TABLE_SIZE] = { 0 };
static int party_map_commands[VOTES_TABLE_SIZE] = { 0 };

void statistics_add(char * party) {
    if (strlen(party) == 0) {
        return;
    }
    
    for (int i=0;i<VOTES_TABLE_SIZE;i++) {
        if (party_map_keys[i] == NULL) {
            party_map_keys[i] = strdup(party);
            party_map_commands[i] = 1;
            return;
        }
        
        if (strcmp(party_map_keys[i], party) == 0) {
            party_map_commands[i]++;
            return;
        }
    }
}

int statistics_find(char * party) {
    for (int i=0;i<VOTES_TABLE_SIZE;i++) {
        if (party_map_keys[i] != NULL && strcmp(party_map_keys[i], party) == 0) {
            return party_map_commands[i];
        }
    }
    return 0;
}

char * statistics_remove(char * party) {
    for (int i=0;i<VOTES_TABLE_SIZE;i++) {
        if (party_map_keys[i] != NULL && strcmp(party_map_keys[i], party) == 0) {
            free(party_map_keys[i]);
            party_map_commands[i] = 0;
            party_map_keys[i] = 0;
            break;
        }
    }
    return NULL;
}

void statistics_print(FILE * fp) {
    for (int i=0;i<VOTES_TABLE_SIZE;i++) {
        if (party_map_keys[i] == NULL) {
            continue;
        }
        
        fprintf(fp, "%-20s %-20d \n", party_map_keys[i], party_map_commands[i]);
    }
}