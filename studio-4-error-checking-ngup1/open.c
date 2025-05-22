#include <fcntl.h>  
#include <unistd.h>   
#include <stdio.h>    

#define BUFFER_SIZE 200

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
        exit(-1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    if (bytesRead == -1) {
        perror("Error reading file");
        exit(-1);
    }

    close(fd);
    return 0;
}
