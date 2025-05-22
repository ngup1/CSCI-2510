#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/test_socket"
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    printf("Connected to server!\n");
    printf("Enter a filename to retrieve, 'ls' to list files, or 'quit' to exit:\n");
    
    while (1) {
        sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_LOCAL;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            perror("connect");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("\nEnter command: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }
    
        if (write(sockfd, buffer, strlen(buffer)) < 0) {
            perror("write");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
     
        if (strncmp(buffer, "quit", 4) == 0) {
            close(sockfd);
            break;
        }

        ssize_t bytes_read;
        while ((bytes_read = read(sockfd, buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }
        
        if (bytes_read < 0) {
            perror("read");
        }
        
        close(sockfd);
    }

    return 0;
} 