#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

void ignore_sigint(int signum) {
    printf("Ignoring SIGINT (Ctrl+C) in caller!\n");
}

int main(int argc, char* argv[]) {
    signal(SIGINT, ignore_sigint); // Register handler
    pid_t ret = fork();

    if (ret == -1) {
        perror("Could not fork");
        exit(-1);
    } 

    if (ret == 0) { // Child
        char* cmd = "./sleep";
        char* myargv[] = {"sleep", NULL};
        execvp(cmd, myargv);
        perror("execvp failed");
        exit(-1);
    }

    printf("Waiting for sleeper %d...\n", ret);
    waitpid(ret, NULL, 0);
    printf("Parent finished waiting!\n");
    return 0;
}