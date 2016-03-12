#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define usersLimit 16

struct sockaddr_in socketAddress;
int fd, connected[usersLimit];

static void *listenUser(void* id){
    int current_client = *((int*)id);
    
    while (1){
        
        int nameLen;
        if ( recv(connected[current_client],
                  (void*)&nameLen,
                  sizeof(int),
                  0) <= 0)
        {
            break;
        }
        
        char* name = (char*)malloc(nameLen*sizeof(char));
        recv(connected[current_client],
             (void*)name,
             nameLen,
             0);
        int dataSize;
        recv(connected[current_client],
             (void*)&dataSize,
             sizeof(int),
             0);
        char* data = (char*)malloc(dataSize*sizeof(char));
        recv(connected[current_client],
             (void*)data,
             dataSize,
             0);
        FILE* toSave = fopen(name, "w");
        fprintf(toSave, "%s", data);
        fclose(toSave);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    int port;
    sscanf(argv[2], "%d", &port);
    socketAddress.sin_port = htons(port);
    socketAddress.sin_family = AF_INET;

    
    if (strcmp(argv[1], "client") == 0){
        inet_aton(argv[3], &(socketAddress.sin_addr));
        
        while (1){
            int nameLen;
            scanf("%d", &nameLen);
            char* name = (char*)malloc(nameLen*sizeof(char));
            scanf("%s", name);
            int dataSize;
            scanf("%d", &dataSize);
            char* data = (char*)malloc(dataSize*sizeof(char));
            // здесь сделать чтение из файла
            scanf("%s", data);
            void* toSend = malloc(sizeof(int) + nameLen*sizeof(char) + sizeof(int) + dataSize*sizeof(char));
            memcpy(toSend, &nameLen, sizeof(int));
            memcpy(toSend + sizeof(int), name, nameLen);
            memcpy(toSend + sizeof(int) + nameLen*sizeof(char), &dataSize, sizeof(int));
            memcpy(toSend + sizeof(int) + nameLen*sizeof(char) + sizeof(int), data, dataSize);
            
            size_t toSendSize = sizeof(int) + nameLen*sizeof(char) + sizeof(int) + dataSize*sizeof(char);
            connect(fd,
                    (struct sockaddr*)&socketAddress,
                    sizeof(socketAddress) );
            int currentSize = 0;
            ssize_t sentSize;
            while (currentSize < toSendSize){
                sentSize = send(fd,
                                toSend + currentSize,
                                toSendSize - currentSize,
                                0);
                if (sentSize == -1)
                    break;
                currentSize += sentSize;
            }
        }
        
    }else{
        
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(fd,
             (struct sockaddr*)&socketAddress,
             sizeof(socketAddress));
        listen(fd, 16);
        
        for (int i = 0; i < usersLimit; ++i)
            connected[i] = -1;
        
        while(1){
            
            int this_fd = accept(fd, NULL, NULL);
            int thisClientNumber = 0;
            while (thisClientNumber < usersLimit && connected[thisClientNumber] != -1)
                ++thisClientNumber;
            
            connected[thisClientNumber] = this_fd;
v
        }
        
    }
    
    close(fd);
    return 0;
}