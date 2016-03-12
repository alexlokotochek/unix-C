#include <unistd.h>
#include <string.h>
#include <stdio.h>

unsigned short int* convert(unsigned int* A){
    *A %= 256*256;
    return (unsigned short int*)A;
}

int main() {
    unsigned int A;
    scanf("%u", &A);
    unsigned short int* newA = convert(&A);
    printf("%u", *newA);
    return 0;
}
