#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    pid_t child_pid = fork();

    if (child_pid < 0) {
        // fork() failed
        fprintf(stderr, "failed\n");
        return 1;
    }
    pid_t pid = getpid();

    if (child_pid == 0) {
        char* child_argv[] = {"ls", "-l", NULL};
        char* child_prog = child_argv[0];      

        execvp(child_prog, child_argv);

        fprintf(stderr, "Exec failed\n");
        return 1;
    } else {
        printf("Parent Process PID is %d, and my child's PID is %d\n", pid, child_pid);
        waitpid(child_pid, NULL, 0);
        printf("Parent Process: Child process has finished.\n");
    }

    return 0;
}