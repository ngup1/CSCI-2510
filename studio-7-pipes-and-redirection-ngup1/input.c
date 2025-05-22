#include <stdio.h>

int main() {
    char input[1024];  
    while(fgets(input, sizeof(input), stdin) != NULL) {
        printf("Program 2 got: %s", input);
    }
    return 0;
}
