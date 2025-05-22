#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/test_socket"
#define BUFFER_SIZE 256

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH); // remove existing sockets

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s\n", SOCKET_PATH);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }
        printf("New connection established\n");

        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            close(client_fd);
            continue;
        }
        
        buffer[bytes_read] = '\0';
        
        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
    
        if (strcmp(buffer, "quit") == 0) {
            printf("Server shutting down\n");
            close(client_fd);
            close(server_fd);
            unlink(SOCKET_PATH);
            exit(EXIT_SUCCESS);
        }
        
        if (strcmp(buffer, "ls") == 0) {
            FILE *ls_output = popen("ls", "r");
            if (!ls_output) {
                const char *error_msg = "Error executing ls command\n";
                write(client_fd, error_msg, strlen(error_msg));
            } else {
                char ls_buffer[BUFFER_SIZE];
                while (fgets(ls_buffer, BUFFER_SIZE, ls_output) != NULL) {
                    write(client_fd, ls_buffer, strlen(ls_buffer));
                }
                pclose(ls_output);
            }
            shutdown(client_fd, SHUT_RDWR);
            close(client_fd);
            continue;
        }
        
        FILE *file = fopen(buffer, "r");
        if (!file) {
            const char *error_msg = "Error: File not found or cannot be opened\n";
            write(client_fd, error_msg, strlen(error_msg));
        } else {
            char file_buffer[BUFFER_SIZE];
            while (fgets(file_buffer, BUFFER_SIZE, file) != NULL) {
                write(client_fd, file_buffer, strlen(file_buffer));
            }
            fclose(file);
        }
        
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }
    
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
} 