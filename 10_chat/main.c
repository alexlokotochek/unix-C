#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

char buf[512];
int users[65]; // 0-terminated, достаточно указать здесь
struct sockaddr_in myAddress;
int sock;

void doDirtyJob( ssize_t (*funcPointer)( int, void*, size_t, int ),
                int thisSock,
                char *buf,
                ssize_t length )
{
    ssize_t readAll = 0, readNow;
    
    while(readAll < length)
    {
        readNow = funcPointer(thisSock, buf + readAll, length - readAll, 0);
        if (readNow == -1)
            return;
        readAll += readNow;
    }
    
}

void* serverThreadFunc( void* user ){
    
    int me = *( (int*)user );
    printf( "connected user: %d\n", me );
    
    while(1){
        ssize_t length;
        ssize_t rc = recv(users[me], (void*) &length, sizeof( ssize_t ), 0);
        if (rc <= 0){
            printf( "disconnected user: %d\n", me );
            users[me] = -1;
            break;
        }
        doDirtyJob( recv, users[me], buf, length );
        buf[length] = '\0';
        printf( "user %d message: %s\n", me, buf );
        int i;
        while (users[i] != 0){
            if (i != me && users[i] != -1){
                doDirtyJob( send, users[i], (void*) &length, sizeof( ssize_t ) );
                doDirtyJob( send, users[i], buf, length );
            }
            ++i;
        }
    }
    pthread_exit( NULL );
    
}


void* clientThreadFunc( void* nothing ){
    
    while(1){
        connect( sock, (struct sockaddr *) &myAddress, sizeof( struct sockaddr ) );
        ssize_t length = 0;
        recv( sock, (void*) &length, sizeof (ssize_t ), 0 );
        
        if (length < 0)
            break;
        
        doDirtyJob( recv, sock, buf, length );
        
        buf[length] = '0';
        printf( "message recieved: %s\n", buf );
    }
    
    pthread_exit( NULL );
    
}


int main( int argc, const char * argv[] ) {
    //server, port ---OR--- client, port, ip
    
    int sock = socket( AF_INET, SOCK_STREAM, 0 );
    myAddress.sin_family = AF_INET;
    myAddress.sin_port = htons( atoi(argv[2]) );
    
    if (strcmp( argv[1], "server" ) == 0){
        
        myAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        bind( sock, (struct sockaddr*)&myAddress, sizeof( struct sockaddr_in ) );
        listen( sock, 64 );
        
        printf( "Server zapushen\n" );
        
        for (int i = 0; i < 20; ++i)
            users[i] = -1;
        users[20] = 0;
        
        while(1){
            int userSock = accept( sock, NULL, NULL );
            int i = 0;
            while(users[i] != 0 && users[i] != -1)
                ++i;
            if (users[i] == 0){
                printf( "Too many users!\n" );
                continue;
            }
            users[i] = userSock;
            pthread_t serverThread;
            pthread_create(&serverThread, NULL, serverThreadFunc, (void*)&i);
        }
    }
    
    if (strcmp( argv[1], "client" ) == 0){
        
        pthread_t clientThread;
        pthread_create(&clientThread, NULL, clientThreadFunc, NULL);
        char input[512];
        
        while(1){
            gets( input );
            if (strcmp( input, "exit" ) == 0){
                pthread_cancel( clientThread );
                pthread_join( clientThread, NULL );
                break;
            }
            ssize_t length = strlen(input);
            connect( sock, (struct sockaddr*) &myAddress, sizeof( myAddress ) );
            doDirtyJob( send, sock, (void*) &length, sizeof ( ssize_t ) );
            doDirtyJob( send, sock, input, length );
        }
        
    }
    
    close(sock);
    return 0;
}
