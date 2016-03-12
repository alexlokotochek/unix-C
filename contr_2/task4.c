#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

int taskSize;
int* strCount;
int* wordCount;
char* text;
ssize_t fileSize;

void* countTask (void* arg){
    int num = *((int*)arg);
    char c;
    char* str = (char*)malloc(taskSize*sizeof(char));
    strncpy(str, text+num*taskSize, taskSize);

    for (int i = 0; i < taskSize; ++i){
        if (str[i] == '\n')
            ++strCount[num];
    }
    if (strlen(str)!=0)
        ++strCount[num];
    
    int counter, flag = 1;
    size_t current = 0, strLen = strlen(str);
    while (current < strLen){
        c = str[current];
        if ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c=='_') || (c>='0' && c<='9') ){
            if (flag == 0)
                counter++;
            flag = 1;
        }else
            flag = 0;
        ++current;
    }
    wordCount[num] += counter;
    
    free(str);
    pthread_exit(NULL);
}


int main(int argc, const char * argv[]) {
    setbuf(stdout, 0);
    printf("%s\n", argv[1]);
    strCount = 0;
    wordCount = 0;
    
    int file_fd = open(argv[1], O_RDWR);
    struct stat entryInfo;
    lstat(argv[1], &entryInfo);
    fileSize = entryInfo.st_size;
    text = (char*)mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, file_fd, 0);
    //
    //
    int threadsNum = atoi(argv[2]);
    if (threadsNum == 0)
        exit(EXIT_FAILURE);
    taskSize = (int)fileSize/threadsNum;
    
    wordCount = (int*)malloc(threadsNum*sizeof(int));
    strCount = (int*)malloc(threadsNum*sizeof(int));
    
    pthread_t* threads = (pthread_t*)malloc(threadsNum*sizeof(pthread_t));
    
    for (int i = 0; i < threadsNum; ++i){
        wordCount[i] = 0;
        strCount[i] = 0;
        pthread_create(&threads[i], NULL, countTask, (void*)&i);
    }
    
    
    int strAns = 0, wordAns = 0;
    for (int i = 0; i < threadsNum; ++i){
        pthread_join(threads[i], NULL);
        strAns += strCount[i];
        wordAns += wordCount[i];
    }
    
    int rest = (int) (fileSize - fileSize/threadsNum), restStr = 0, restWords = 0;
    if (rest > 0){
        char c;
        char* str = (char*)malloc(rest*sizeof(char));
        strncpy(str, text+threadsNum*taskSize, rest);
        
        for (int i = 0; i < rest; ++i){
            if (str[i] == '\n')
                ++restStr;
        }
        if (strlen(str)!=0)
            ++restStr;
        
        int counter, flag = 1;
        size_t current = 0, strLen = strlen(str);
        while (current < strLen){
            c = str[current];
            if ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c=='_') || (c>='0' && c<='9') ){
                if (flag == 0)
                    counter++;
                flag = 1;
            }else
                flag = 0;
            ++current;
        }
        restWords += counter;
        free(str);
    }
    munmap(text, fileSize);

    strAns += restStr;
    wordAns += restWords;
    
    printf("\nstrings:%d\nwords:%d", strAns, wordAns);
    return 0;
}
