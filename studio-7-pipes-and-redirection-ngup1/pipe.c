
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pipe(fd); 

    pid_t pid1 = fork();
    if (pid1 == 0){
        dup2(fd[1], STDOUT_FILENO); 
        close(fd[0]); 
        close(fd[1]); 
        char* args1[] = {"./print", NULL};
        execvp(args1[0], args1);
        perror("exec failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO); 
        close(fd[1]);
        close(fd[0]);
        char* args2[] = {"./input", NULL};
        execvp(args2[0], args2);
        perror("exec failed");
        exit(1);
    }
    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}


