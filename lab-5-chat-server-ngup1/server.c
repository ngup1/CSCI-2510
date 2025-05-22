// Usage: ./server <port_number>

// This server should listen on the specified port, waiting for incoming connections.
// After a client connects, add it to a list of currently connected clients.
// If any message comes in from *any* connected client, then it is repeated to *all*
// other connected clients.
// If reading or writing to a client's socket fails, then that client should be removed
// from the linked list.

// Remember that blocking read calls will cause your server to stall. Instead, set your
// your sockets to be non-blocking. Then, your reads will never block, but instead return
// an error code indicating there was nothing to read- this error code can be either
// EAGAIN or EWOULDBLOCK, so make sure to check for both. If your read call fails
// with that error, then ignore it. If it fails with any other error, then treat that
// client as though they have disconnected.

// You can create non-blocking sockets by passing the SOCK_NONBLOCK argument to both
// the socket() function, as well as the accept4() function.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct client {
    int socket;
    char name[32];
    struct client* next;
} client_t;

client_t* clients = NULL;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function declarations
void broadcast_message(const char* message, int sender_socket);
void remove_client(int socket);
void* handle_client(void* arg);

void broadcast_message(const char* message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    client_t* current = clients;
    while (current != NULL) {
        if (current->socket != sender_socket) {
            if (send(current->socket, message, strlen(message), 0) < 0) {
                perror("Failed to send message");
                remove_client(current->socket);
            }
        }
        current = current->next;
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int socket) {
    pthread_mutex_lock(&clients_mutex);
    client_t* current = clients;
    client_t* prev = NULL;

    while (current != NULL) {
        if (current->socket == socket) {
            if (prev == NULL) {
                clients = current->next;
            } else {
                prev->next = current->next;
            }
            close(current->socket);
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    int socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    client_t* current_client = NULL;
    int user_num = 1;
    char default_name[32];

    // Find the client in our list and set up their default name
    pthread_mutex_lock(&clients_mutex);
    current_client = clients;
    while (current_client != NULL) {
        if (current_client->socket == socket) {
            sprintf(default_name, "User%d", user_num);
            strcpy(current_client->name, default_name);
            sprintf(message, "%s has connected\n", current_client->name);
            break;
        }
        current_client = current_client->next;
        user_num++;
    }
    pthread_mutex_unlock(&clients_mutex);

    // Broadcast the connection message
    broadcast_message(message, socket);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            break;
        }

        // Handle name change command
        if (strncmp(buffer, "name ", 5) == 0) {
            char old_name[32];
            pthread_mutex_lock(&clients_mutex);
            current_client = clients;
            while (current_client != NULL) {
                if (current_client->socket == socket) {
                    strcpy(old_name, current_client->name);
                    strcpy(current_client->name, buffer + 5);
                    current_client->name[strcspn(current_client->name, "\n")] = 0;
                    sprintf(message, "%s has changed their name to %s\n", old_name, current_client->name);
                    break;
                }
                current_client = current_client->next;
            }
            pthread_mutex_unlock(&clients_mutex);
            broadcast_message(message, socket);
        }
        // Handle quit command
        else if (strncmp(buffer, "quit", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            current_client = clients;
            while (current_client != NULL) {
                if (current_client->socket == socket) {
                    sprintf(message, "%s has quit\n", current_client->name);
                    break;
                }
                current_client = current_client->next;
            }
            pthread_mutex_unlock(&clients_mutex);
            broadcast_message(message, socket);
            break;
        }
        // Handle regular message
        else {
            pthread_mutex_lock(&clients_mutex);
            current_client = clients;
            while (current_client != NULL) {
                if (current_client->socket == socket) {
                    sprintf(message, "%s: %s", current_client->name, buffer);
                    break;
                }
                current_client = current_client->next;
            }
            pthread_mutex_unlock(&clients_mutex);
            broadcast_message(message, socket);
        }
    }

    remove_client(socket);
    free(arg);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Port number must be between 1024 and 65535\n");
        exit(EXIT_FAILURE);
    }

    // Create server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    int flags = fcntl(server_socket, F_GETFL, 0);
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            perror("Accept failed");
            continue;
        }

        // Set client socket to non-blocking mode
        flags = fcntl(client_socket, F_GETFL, 0);
        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

        // Add new client to the list
        client_t* new_client = (client_t*)malloc(sizeof(client_t));
        if (new_client == NULL) {
            perror("Memory allocation failed");
            close(client_socket);
            continue;
        }

        new_client->socket = client_socket;
        new_client->next = NULL;

        pthread_mutex_lock(&clients_mutex);
        new_client->next = clients;
        clients = new_client;
        pthread_mutex_unlock(&clients_mutex);

        // Create thread to handle client
        pthread_t thread;
        int* client_socket_ptr = (int*)malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        if (pthread_create(&thread, NULL, handle_client, client_socket_ptr) != 0) {
            perror("Thread creation failed");
            free(client_socket_ptr);
            remove_client(client_socket);
        }
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}
