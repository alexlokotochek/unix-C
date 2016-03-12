#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>


int main(int argc, const char * argv[]) {
    setbuf(stdout, 0);
    
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sock;
    inet_aton(argv[1], &(sock.sin_addr));
    sock.sin_port = htons(atoi(argv[2]));
    sock.sin_family = AF_INET;
    connect(sock_fd, (struct sockaddr*)&sock, sizeof(sock));
    
    int file_fd = open(argv[3], O_RDONLY);
    struct stat entryInfo;
    lstat(argv[3], &entryInfo);
    char* text = (char*)mmap(NULL, entryInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_fd, 0);
    
    
    char* strToSend = strtok (text,"\n\0");
    char* strToCheck = strtok(NULL, "\n\0");
    char* input = (char*)malloc(1<<10*sizeof(char));
    
    ssize_t strLen = 0, currentPos = 0, partLen;
    int strNum = 0;
    
    while (strToSend != NULL){
        //отправляем полностью "первую" строку
        strLen = strlen(strToSend);
        while (currentPos < strLen){
            partLen = send(sock_fd, strToSend + currentPos, strLen, 0);
            currentPos += partLen;
            if (partLen == -1)
                break;
        }
        
        strLen = strlen(strToCheck);
        //теперь считываем "вторую"
        currentPos = 0;
        input[0] = 'a';
        while (input[currentPos] != '\n'){
            partLen = recv(sock_fd, input + currentPos, strLen, 0);
            currentPos += partLen;
            if (partLen == -1)
                break;
            if (currentPos > strLen){
                exit(EXIT_FAILURE);
            }
        }
        
        //сравниваем
        ++strNum;
        if (strcmp(input, strToCheck) != 0)
            exit(EXIT_FAILURE);
        else
            printf("\n%d", strNum);
        
        strToSend = strtok (NULL, "\n\0");
        strToCheck = strtok(NULL, "\n\0");
    }
    
    return 0;
}
