#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_PROCS 15
#define NUM_RES 15

int processes_to_resources[NUM_PROCS][NUM_RES];
int resource_holder[NUM_RES];

void init_matrix(void);
void acquire(int process, int resource);
int release(int resource);
int start_search(int start_process);
int search_resource(int visited[NUM_PROCS], int start, int current);
int search_process(int visited[NUM_PROCS], int start, int current);
void print_cycle(int visited[NUM_PROCS], int start);

int main(void) {
    int process, resource;
    char action;
    
    // Initialize the RAG
    init_matrix();
    
    // Read input until the end of the file
    while (scanf("%d %c %d", &process, &action, &resource) == 3) {
        if (process < 0 || process >= NUM_PROCS || resource < 0 || resource >= NUM_RES) {
            fprintf(stderr, "Invalid process or resource number\n");
            return 1;
        }
        
        if (action == 'a') {
            acquire(process, resource);
            if (start_search(process)) {
                print_cycle(NULL, process);
                return -1;
            }
        } else if (action == 'r') {
            int next_process = release(resource);
            if (next_process != -1 && start_search(next_process)) {
                print_cycle(NULL, next_process);
                return -1;
            }
        } else {
            fprintf(stderr, "Invalid action: %c\n", action);
            return 1;
        }
    }
    
    return 0;
}

void init_matrix(void) {
    for (int i = 0; i < NUM_PROCS; i++) {
        for (int j = 0; j < NUM_RES; j++) {
            processes_to_resources[i][j] = 0;
        }
    }
    
    for (int i = 0; i < NUM_RES; i++) {
        resource_holder[i] = -1;
    }
}

void acquire(int process, int resource) {
    if (resource_holder[resource] == -1) {
        resource_holder[resource] = process;
    } else {
        processes_to_resources[process][resource] = 1;
    }
}

int release(int resource) {
    int holder = resource_holder[resource];
    if (holder == -1) {
        return -1;
    }
    
    // Find lowest numbered process waiting for this resource
    int next_process = -1;
    for (int i = 0; i < NUM_PROCS; i++) {
        if (processes_to_resources[i][resource]) {
            next_process = i;
            break;
        }
    }
    
    resource_holder[resource] = -1;
    
    if (next_process != -1) {
        processes_to_resources[next_process][resource] = 0;
        resource_holder[resource] = next_process;
    }
    
    return next_process;
}

int start_search(int start_process) {
    int visited[NUM_PROCS] = {0};
    return search_process(visited, start_process, start_process);
}

int search_resource(int visited[NUM_PROCS], int start, int current) {
    int holder = resource_holder[current];
    if (holder == -1) {
        return 0;
    }
    
    if (holder == start) {
        return 1;
    }
    
    return search_process(visited, start, holder);
}

int search_process(int visited[NUM_PROCS], int start, int current) {
    if (visited[current]) {
        return 0;
    }
    
    visited[current] = 1;
    
    // Check all resources this process is waiting for
    for (int i = 0; i < NUM_RES; i++) {
        if (processes_to_resources[current][i]) {
            if (search_resource(visited, start, i)) {
                return 1;
            }
        }
    }
    
    visited[current] = 0;
    return 0;
}

void print_cycle(int visited[NUM_PROCS], int start) {
    // Simplified version that just prints current state
    // Might want to track and print the actual cycle
    for (int i = 0; i < NUM_RES; i++) {
        if (resource_holder[i] != -1) {
            printf("RESOURCE %d -> PROCESS %d\n", i, resource_holder[i]);
        }
    }
    
    for (int i = 0; i < NUM_PROCS; i++) {
        for (int j = 0; j < NUM_RES; j++) {
            if (processes_to_resources[i][j]) {
                printf("PROCESS %d -> RESOURCE %d\n", i, j);
            }
        }
    }
}
