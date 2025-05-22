#define _XOPEN_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <2 char salt> <password>\n", argv[0]);
        return -1;
    }

    char* salt = argv[1];
    char* key = argv[2];
    char* result;

    result = crypt(key, salt);
    if (result == NULL) {
        perror("crypt() failed");
        return -1;
    }

    printf("Salt: %s\nPassword: %s\nHash: %s\n", salt, key, result);
    return 0;
}
