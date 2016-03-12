#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <math.h>

int main(int argc, const char * argv[]) {
    setbuf(stdout, 0);
    
    int fd = open(argv[1], O_RDONLY);
    struct stat entryInfo;
    lstat(argv[1], &entryInfo);
    ssize_t fileSize = entryInfo.st_size;
    
    int freq[256];
    for (int i = 0; i < 256; ++i)
        freq[i]=0;
    
    char* data = (char*)mmap(NULL, entryInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    int a, b;
    long double sum = 0;
    
    while(scanf("%d %d", &a, &b) != EOF){
        for (int i = a; i < b; ++i){
            ++freq[data[i]];
        }
        
        for (int i = 0; i < 256; ++i){
            if (freq[i] != 0)
                sum += (freq[i]/(b-a))*log(freq[i]/(b-a));
        
            freq[i]=0;
        }
        sum = 0;
        printf("\n%Lf", sum);
    }
    
    munmap(data, fileSize);
    return 0;
}
