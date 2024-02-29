
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"
#include "history.h"
#include "execute.h"

int main(int argc, char** argv) {
    size_t bufsize = 1024;
    size_t characters = 0;
    char *command_buffer = (char *) malloc(bufsize * sizeof (char));
    
    if (command_buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }
    
    setup_hooks();
    
    do {
        strcpy(command_buffer, "");
        
        printf("in-mysh-now:> ");
        
        characters = getline(&command_buffer,&bufsize,stdin);
        
        if (characters > 0 && strlen(command_buffer) > 0) {
            clean(command_buffer);
            
            printf("Command line: '%s' \n", command_buffer);
            
            history_add(command_buffer);
            
            logical_execute(command_buffer);
        }
    } while (characters > 0 && !feof(stdin) && strcmp(command_buffer, "exit") != 0);


    free(command_buffer);
    
    return 0;
}

