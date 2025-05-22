// Usage: ./client <IP_address> <port_number>

// The client attempts to connect to a server at the specified IP address and port number.
// The client should simultaneously do two things:
//     1. Try to read from the socket, and if anything appears, print it to the local standard output.
//     2. Try to read from standard input, and if anything appears, print it to the socket. 

// Remember that you can use a pthread to accomplish both of these things simultaneously.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int socket_fd;
char username[32] = "User";

void* receive_messages(void* arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            printf("Server shut down unexpectedly\n");
            exit(EXIT_FAILURE);
        }
        
        printf("%s", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_address> <port_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    int flags = fcntl(socket_fd, F_GETFL, 0);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

    // Connect to server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Start connection
    int ret = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        if (errno != EINPROGRESS) {
            perror("Connection failed");
            exit(EXIT_FAILURE);
        }
        
        // Wait for connection to complete
        fd_set write_fds;
        struct timeval timeout;
        
        FD_ZERO(&write_fds);
        FD_SET(socket_fd, &write_fds);
        timeout.tv_sec = 5;  // 5 second timeout
        timeout.tv_usec = 0;
        
        ret = select(socket_fd + 1, NULL, &write_fds, NULL, &timeout);
        if (ret == 0) {
            fprintf(stderr, "Connection timeout\n");
            exit(EXIT_FAILURE);
        } else if (ret < 0) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }
        
        // Check if connection was successful
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            perror("Getsockopt failed");
            exit(EXIT_FAILURE);
        }
        if (error != 0) {
            fprintf(stderr, "Connection failed: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    // Create thread to receive messages
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, NULL) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    // Main loop for sending messages
    char buffer[BUFFER_SIZE];
    while (1) {
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Remove newline from the end of the input
        buffer[strcspn(buffer, "\n")] = 0;

        // Handle name command
        if (strncmp(buffer, "name ", 5) == 0) {
            char* new_name = buffer + 5;
            if (strlen(new_name) > 0) {
                strcpy(username, new_name);
                // Send the name command to server
                if (send(socket_fd, buffer, strlen(buffer), 0) < 0) {
                    perror("Failed to send name change");
                    break;
                }
            }
        }
        // Handle quit command
        else if (strcmp(buffer, "quit") == 0) {
            if (send(socket_fd, "quit", 4, 0) < 0) {
                perror("Failed to send quit message");
            }
            break;
        }
        // Handle regular message
        else {
            // Send message to server
            if (send(socket_fd, buffer, strlen(buffer), 0) < 0) {
                perror("Failed to send message");
                break;
            }
        }
    }

    close(socket_fd);
    return 0;
}
