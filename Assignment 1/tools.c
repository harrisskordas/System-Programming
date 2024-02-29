
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tools.h"


void clean(char * data) {
    while (*data != '\0') {
        if (*data == '\n') {
            *data = '\0';
        }
        if (*data == '\r') {
            *data = '\0';
        }        
        data++;
    }    
}

void unquote(char * data) {
    while (*data != '\0') {
        if (*data == '"') {
            *data = '\0';
        }        
        data++;
    }  
}

char ** tokenize(char * data, char * delim) {
    int tokens = 0;
    
    for (int i=0;i<strlen(data);i++) {
        if (data[i] == *delim) {
            tokens++;
        }
    }
    
    int array_size = tokens + 2;
    
    char ** array = malloc(sizeof(char*)*array_size);
    
    for (int i=0;i<array_size;i++) {
        array[i] = NULL;
    }
    
    array[0] = strtok(data, delim);
    
    for (int i=1;i<=tokens;i++) {
        array[i] = strtok(NULL, delim);
    }
    
    for (int i=0;i<=tokens;i++) {
        while (*(array[i]) == ' ') {
            array[i]++;
        }
        
        while (array[i][strlen(array[i]) - 1] == ' ') {
            array[i][strlen(array[i]) - 1] = '\0';
        }
    }
    
    return array;
}

void dumpTokens(char * header, char ** tokens) {
    char ** tmp = tokens;
    
    printf("%s: \n" , header);
    
    while (*tmp != NULL) {
        printf(" - '%s' \n", *tmp);
        tmp++;
    }
}

int countPipes(char * data) {
    int c = 0;
    
    for (int i=0;i<strlen(data);i++) {
        if (data[i] == '|') {
            c++;
        }
    }
    return c;
}