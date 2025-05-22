
/*

#include <stdio.h>
#include <string.h>

int main(){
    
    char *messagePtr = "HELLOWORLD!";
    printf("%c\n", *(messagePtr + 1));
}

#include <stdio.h>
#include <string.h>

int main(){
    char i;
    char *messagePtr = "HELLOWORLD!";
    for (i = 0; i < strlen(messagePtr); i++){
        printf("%c\n", *(messagePtr + i));
    }
}

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* reverseString(char* input) {
    int length = 0;
    while (input[length] != '\0') {
        length++;
    }
    char* output = (char*)malloc(length + 1);
    if (output == NULL) { 
        return NULL;
    }
    for (int i = 0; i < length; i++) {
        output[i] = input[length - 1 - i];
    }
    output[length] = '\0';
    return output;
}

int main() {
    char *messagePtr = "HELLOWORLD!";
    char* reversedMessage = reverseString(messagePtr);
    printf("Reversed string: %s\n", reversedMessage);
}
