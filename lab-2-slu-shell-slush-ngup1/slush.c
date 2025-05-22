//SLUSH - The SLU SHELL
//
//This assignment reads user input, and executes that input as a sequence of
//programs to execute.
//
//Program execution commands have the form:
//
//prog_n [args] ( ... prog_3 [args] ( prog_2 [args] ( prog_1 [args]
//
//For example, "grep slush ( sort -r ( ls -l -a" returns a reverse sorted list
//of all files in the current directory that contain the string "slush" in
//
//See the lab writeup for more details and other requirements.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT 256
#define MAX_ARGS 16
#define MAX_COMMANDS 10

void handleSigint(int sig){
    printf("\n");
    printf("slush> ");
    fflush(stdout);
}

void executeCommand(char *input){
    if(strncmp(input, "cd ", 3) == 0){
        char *dir = input + 3;
        while (*dir == ' '){
            dir++;
        } 
        if(chdir(dir) == -1){
            perror("cd");
        }
        return;
    }
    printf("Command not found: %s\n", input);
}

void executePipeline(char *commands[], int num_commands){
    int pipes[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    for(int i = 0; i < num_commands - 1; i++){
        if(pipe(pipes[i]) == -1){
            perror("pipe");
            exit(1);
        }
    }

    for(int i = 0; i < num_commands; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork");
            exit(1);
        }
        if(pids[i] == 0){ 
            if(i > 0){
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if(i < num_commands - 1){
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for(int j = 0; j < num_commands - 1; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            char *args[MAX_ARGS];
            int arg_index = 0;
            char *token = strtok(commands[i], " ");
            while(token != NULL && arg_index < MAX_ARGS - 1){
                args[arg_index++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_index] = NULL;
            execvp(args[0], args);
            perror(args[0]); //if fails
            exit(1);
        }
    }
    for(int i = 0; i < num_commands - 1; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for(int i = 0; i < num_commands; i++){
        waitpid(pids[i], NULL, 0);
    }
}

void updatePrompt(){
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    char *home = getenv("HOME");
    if(home != NULL && strncmp(cwd, home, strlen(home)) == 0){
        printf("slush|%s> ",cwd + strlen(home));
    }else{
        printf("slush|%s> ", cwd);
    }
}

int main(){
    struct sigaction sa;
    sa.sa_handler = handleSigint;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    char input[MAX_INPUT];

    while(1){
        updatePrompt();
        if(fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\nExiting slush.\n");
            break;
        }
        input[strcspn(input, "\n")] = 0;
        if(strlen(input) == 0){
            continue; 
        }
        char *commands[MAX_COMMANDS];
        int num_commands = 0;
        char *token = strtok(input, "(");
        while(token != NULL && num_commands < MAX_COMMANDS){
            while(*token == ' '){
                token++; 
            }
            commands[num_commands++] = token;
            token = strtok(NULL, "(");
        }
        if(num_commands == 0){
            continue;
        }
        if(strncmp(commands[0], "cd ", 3) == 0){
            executeCommand(commands[0]);
        }else{
            executePipeline(commands, num_commands);
        }
    }
    return 0;
}
