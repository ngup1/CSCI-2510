
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/test_socket"

int main() {
    int sockfd;
    struct sockaddr_un addr;
    char buffer[256];

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

    printf("Connected to server! Type a message (or 'quit' to exit):\n");
    while (fgets(buffer, sizeof(buffer), stdin)) {
        write(sockfd, buffer, strlen(buffer));
        if (strncmp(buffer, "quit", 4) == 0) break;
    }

    close(sockfd);
    return 0;
}
