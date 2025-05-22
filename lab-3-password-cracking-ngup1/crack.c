#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define CHARSET "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_LEN 26
#define MAX_KEYSIZE 8

char* target_hash;
char salt[3];
int keysize;
int password_found = 0;
pthread_mutex_t found_mutex;
pthread_mutex_t crypt_mutex; 

void generate_passwords(char* prefix, int length, int max_length) {
    if (password_found) return;

    if (length == max_length) {
        pthread_mutex_lock(&crypt_mutex); 
        char* result = crypt(prefix, salt);
        pthread_mutex_unlock(&crypt_mutex);

        if (strcmp(result, target_hash) == 0) {
            pthread_mutex_lock(&found_mutex);
            if (!password_found) {
                password_found = 1;
                printf("Password found: %s\n", prefix);
            }
            pthread_mutex_unlock(&found_mutex);
        }
        return;
    }

    for (int i = 0; i < CHARSET_LEN; i++) {
        if (password_found) return;

        char new_prefix[MAX_KEYSIZE + 1];
        snprintf(new_prefix, sizeof(new_prefix), "%s%c", prefix, CHARSET[i]);
        generate_passwords(new_prefix, length + 1, max_length);
    }
}

void* thread_worker(void* arg) {
    int thread_id = *((int*)arg);
    int total_threads = *((int*)(arg + sizeof(int)));

    for (int len = 1; len <= keysize; len++) {
        char prefix[MAX_KEYSIZE + 1] = {0};

        for (int i = thread_id; i < CHARSET_LEN; i += total_threads) {
            prefix[0] = CHARSET[i];
            generate_passwords(prefix, 1, len);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <threads> <keysize> <target>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    keysize = atoi(argv[2]);
    target_hash = argv[3];

    if (keysize > MAX_KEYSIZE || keysize < 1) {
        fprintf(stderr, "Error: Keysize must be between 1 and %d\n", MAX_KEYSIZE);
        return 1;
    }

    strncpy(salt, target_hash, 2);
    salt[2] = '\0';

    pthread_t thread_ids[num_threads];
    pthread_mutex_init(&found_mutex, NULL);
    pthread_mutex_init(&crypt_mutex, NULL);

    for (int i = 0; i < num_threads; i++) {
        int* args = malloc(2 * sizeof(int));
        args[0] = i;
        args[1] = num_threads;
        pthread_create(&thread_ids[i], NULL, thread_worker, args);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    pthread_mutex_destroy(&found_mutex);
    pthread_mutex_destroy(&crypt_mutex);

    return 0;
}
