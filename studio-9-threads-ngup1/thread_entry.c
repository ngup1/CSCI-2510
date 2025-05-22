#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 5

typedef struct {
    int arg1;
    char arg2[100];
    int ret;
} ThreadData;

void* thread_entry(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    printf("Thread %d: %s\n", data->arg1, data->arg2);
    data->ret = data->arg1 * 2; // Example computation
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        data[i].arg1 = i;
        snprintf(data[i].arg2, sizeof(data[i].arg2), "This is thread %d", i);

        if (pthread_create(&threads[i], NULL, thread_entry, (void *)&data[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d returned value: %d\n", i, data[i].ret);
    }

    return 0;
}
