#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, const char * argv[]) {
    setbuf(stdout, 0);
    in_addr_t ip1 = inet_addr(argv[1]);
    in_addr_t ip2 = inet_addr(argv[2]);
    in_addr_t mask = (inet_addr(argv[3]));
    
    if ((ip1 & mask) == (ip2 & mask))
        printf("1");
    else
        printf("0");
    return 0;
}
