#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_RUN_LENGTH 255  

void compress(int input, int output, int k){
    char buffer[k], next_buffer[k];  //two arrays, one to hold the current chunk and one to hold the next chunk and compare. 
    ssize_t bytes_read; //bytes_read needs to be a signed integer type so you can handle both positive values(bytes read), 
    //0 (end of file), and -1 (error) in a type-safe way.
    int count = 1;

    bytes_read = read(input, buffer, k);
    if (bytes_read < k){ //check if the bytes being read are smaller than the chunk (either youve reached the end or it wasnt long enough)
        write(output, &count, 1);//dereference count to pass its address into while()
        write(output, buffer, bytes_read);
        return;
    }

    while((bytes_read = read(input, next_buffer, k)) == k){
        if (memcmp(buffer, next_buffer, k) == 0) {//originally used strcmp from the lab manual but after reading about different ways to 
        //compare in C, this approach made more sense since it compares blocks of memory. 
            count++;
            if (count == MAX_RUN_LENGTH){
                write(output, &count, 1);  
                write(output, buffer, k);  
                count = 0;
            }
        } else {
            write(output, &count, 1);  
            write(output, buffer, k);  
            memcpy(buffer, next_buffer, k); 
            count = 1;
        }
    }
    if (count > 0){
        write(output, &count, 1);  
        write(output, buffer, k);
    }
    if (bytes_read > 0){
        write(output, &count, 1);         
        write(output, next_buffer, bytes_read); 
    }
}

void decompress(int input, int output, int k) {
    char buffer[k];
    unsigned char count;
    ssize_t bytes_read;

    while((bytes_read = read(input, &count, 1)) == 1) {
        bytes_read = read(input, buffer, k);
        if (bytes_read <= 0) {
            fprintf(stderr, "Error: Incomplete or corrupted input file\n");
            break;
        }
        for (int i = 0; i < count; i++) {
            if (write(output, buffer, bytes_read) != bytes_read) {
                perror("Error writing to output file");
                return;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading input file");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input file> <output file> <compression length> <mode>\n", argv[0]);
        return 1;
    }

    int k = atoi(argv[3]);
    if (k < 1){
        fprintf(stderr, "Compression length must be at least 1\n");
        return 1;
    }

    int mode = atoi(argv[4]);
    if (mode != 0 && mode != 1){
        fprintf(stderr, "Mode must be 0 (compress) or 1 (decompress)\n");
        return 1;
    }

    int input = open(argv[1], O_RDONLY);
    if (input < 0){
        perror("Error opening input file");
        return 1;
    }

    int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output < 0){
        perror("Error opening output file");
        close(input);
        return 1;
    }

    if (mode == 0){
        compress(input, output, k);
    } else {
        decompress(input, output, k);
    }

    close(input);
    close(output);
    return 0;
}