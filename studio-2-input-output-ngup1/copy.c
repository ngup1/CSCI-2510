#include <unistd.h>  
#define bufferSize 200

int main() {
    char buffer[ bufferSize ];
    int bytesRead;

    while (1) {
        bytesRead = read(STDIN_FILENO, buffer, bufferSize);

        if (bytesRead <= 0) {
            break;
        }
       write(STDOUT_FILENO, buffer, bytesRead);
    }

    return 0;
}