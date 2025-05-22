#include <stdio.h>
#include <pthread.h>

int race = 0;
pthread_mutex_t mutex;

void *adder(void *arg) {
    pthread_mutex_lock(&mutex);  // Lock the mutex
    for (int i = 0; i < 20000000; i++) { // 20 million iterations
        race++;                      // Modify the shared variable
    }
    pthread_mutex_unlock(&mutex); // Unlock the mutex
    return NULL;
}

void *subtractor(void *arg) {
    pthread_mutex_lock(&mutex);  // Lock the mutex
    for (int i = 0; i < 20000000; i++) { // 20 million iterations
        race--;                      // Modify the shared variable
    }
    pthread_mutex_unlock(&mutex); // Unlock the mutex
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_mutex_init(&mutex, NULL); // Initialize the mutex

    pthread_create(&t1, NULL, adder, NULL);
    pthread_create(&t2, NULL, subtractor, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex); // Clean up the mutex
    printf("Final value of race (concurrent): %d\n", race);
    return 0;
}
