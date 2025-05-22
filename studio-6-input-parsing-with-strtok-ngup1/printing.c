#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT 256
#define MAX_ARGS 15

int main(){
    int max_args = 15;
    int max_argv_size = max_args + 2; //one for argv[0], one for null terminator
    char* cmd;
    char* res;
    char* my_argv[max_argv_size];
    int i;

    char input[MAX_INPUT];
    printf("Enter input command: ");
    if (fgets(input, sizeof(input), stdin) == NULL){
        return 1;
    }
    input[strcspn(input, "\n")] = 0; //find first occurance of newline and set to null
       
    cmd = strtok(input, " ");
    my_argv[0] = cmd;
    i = 1;

    while((res = strtok(NULL, " ")) != NULL && i < MAX_ARGS){
        my_argv[++i] = res;
    }
    my_argv[i + 1] = NULL;
    
    if(execvp(cmd, my_argv) == -1){
        perror("execvp");
    }

    return 0;
}