// Nilesh Gupta
// January 25, 2025
// This program prints a custom message to the console using write().

#include <unistd.h> 

int main(int argc, char* argv[]) {
    // Message to be printed
    char* message = "Hello! This is a custom message from my C program.\n";
    int message_length = 54; 

    write(STDOUT_FILENO, message, message_length);
    return 0;
}
