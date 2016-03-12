#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dispatch/dispatch.h>

dispatch_semaphore_t semFirst;
dispatch_semaphore_t semThird;

struct str{
    int begin;
    int end;
    int free;
};

struct resStr{
    void* begin;
    void* end;
};

char* V(char* string){
    int reallocCounter = 0;
    for (int i = 0; i < strlen(string); ++i){
        if (string[i] >= 'A' && string[i] <= 'Z')
            ++reallocCounter;
    }
    char* tmp = (char*)malloc(strlen(string) + reallocCounter + 1);
    int j = 0;
    for (int i = 0; i < strlen(string); ++i){
        if (string[i] >= 'A' && string[i] <= 'Z'){
            tmp[j] = string[i] - ('a'-'A');
            ++j;
            tmp[j] = string[i] - ('a'-'A');
        }else if (string[i] >= 'a' && string[i] <= 'z'){
            tmp[j] = string[i] + ('a'-'A');
        }else{
            tmp[j] = string[i];
        }
        ++j;
    }
    string = realloc(string, strlen(tmp)+1);
    strcpy(string, tmp);
    free(tmp);
    return string;
}

int numberOfStrings;

struct str* getStringsPositions(char* file){
    int newline_counter = 0;
    char c;
    for (int i = 0; i < strlen(file); ++i){
        sscanf(file, "%c", &c);
        if (c == '\n')
            ++newline_counter;
    }
    struct str* offsets = (struct str*)malloc((newline_counter+1)*sizeof(struct str));
    offsets[0].begin = 0;
    offsets[0].free = 1;
    int j = 0;
    for (int i = 0; i < strlen(file); ++i){
        sscanf(file, "%c", &c);
        if (c == '\n' && i < strlen(file)-2){
            offsets[j].end = i;
            ++j;
            offsets[j].begin = i+1;
            offsets[j].free = 1;
        }
    }
    offsets[j].end = (int)strlen(file)-2;
    numberOfStrings = j+1;
    return offsets;
}

int main(int argc, const char * argv[]) {
    semFirst = dispatch_semaphore_create(1);
    semThird = dispatch_semaphore_create(1);
    int forksCounter = (int)argv[2];
    char* filepath = (char*)argv[1];
    
    int secondPageFile = shm_open(filepath, O_RDWR);
    
    struct stat fileInfo;
    lstat(filepath, &fileInfo);
    char* text = (char*)mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, secondPageFile, 0);
    struct str* stringsPositions = getStringsPositions(text);
    int firstPageFile = shm_open("first_page.txt", O_RDWR | O_CREAT);
    write(firstPageFile, stringsPositions, numberOfStrings*sizeof(struct str));
    
    struct str* first = mmap(NULL, numberOfStrings*sizeof(struct str), PROT_READ | PROT_WRITE, MAP_SHARED, firstPageFile, 0);
    
    int thirdPageFile = shm_open("first_page.txt", O_RDWR | O_CREAT);
    ftruncate(thirdPageFile, fileInfo.st_size*2 + numberOfStrings*64);
    void* third = mmap(NULL, fileInfo.st_size*2 + numberOfStrings*64, PROT_READ | PROT_WRITE, MAP_SHARED, thirdPageFile, 0);
    struct resStr* header = (struct resStr*)malloc(numberOfStrings*sizeof(struct resStr));
    memcpy(third, header, numberOfStrings*sizeof(struct resStr));
    int headerOffset = numberOfStrings*sizeof(struct resStr);
    
    while (forksCounter--){
        if (fork()==0){
            struct str currentReadStr;
            struct resStr currentResStr;
            memcpy(&currentResStr, third, sizeof(struct resStr));
            memcpy(&currentReadStr, &first[0], sizeof(struct str));
            char* currentStr = (char*)malloc(1<<10);
            int firstIter = -1, thirdIter = 0;

            while(1){
                dispatch_semaphore_wait(semFirst, DISPATCH_TIME_FOREVER);
                while (currentReadStr.free != 1){
                    ++firstIter;
                    if (firstIter == numberOfStrings){
                        dispatch_semaphore_signal(semFirst);
                        exit(EXIT_SUCCESS); //exit when amount of free strings ends
                    }
                    memcpy(&currentReadStr, &first[firstIter], sizeof(struct str));
                }
                dispatch_semaphore_signal(semFirst);
                memcpy(currentStr, (void*)first+currentReadStr.begin, currentReadStr.end-currentReadStr.begin);
                currentStr[strlen(currentStr)] = '\0';
                currentStr = V(currentStr);
                struct resStr prevWritten;
                
                dispatch_semaphore_wait(semThird, DISPATCH_TIME_FOREVER);
                while (currentResStr.begin != NULL && thirdIter < numberOfStrings){
                    memcpy(&currentResStr, third + thirdIter*sizeof(struct resStr), sizeof(struct resStr));
                    ++thirdIter;
                }
                if (thirdIter == 0){
                    memcpy(third+headerOffset, &currentStr, sizeof(currentStr));
                    ((struct resStr*)(third))->begin = third+headerOffset;
                    ((struct resStr*)(third))->end = third+headerOffset + sizeof(currentStr);
                }else{
                    prevWritten = *(struct resStr*)(third+(thirdIter-1)*sizeof(struct resStr));
                    memcpy(prevWritten.end, &currentStr, sizeof(currentStr));
                    ((struct resStr*)(third + thirdIter*sizeof(struct resStr)))->begin = prevWritten.end;
                    ((struct resStr*)(third + thirdIter*sizeof(struct resStr)))->end = prevWritten.end + sizeof(currentStr);
                }
                dispatch_semaphore_signal(semThird);
            }
        }else
            break;
    }
    
    while (wait(0) > 0);
    
    char* res = (char*)malloc(1<<10);
    for (int i = 0; i < numberOfStrings; ++i){
        memcpy(res, header[i].begin, header[i].end-header[i].begin);
        strcat(res, '\0');
        printf("%s\n", res);
    }
    
    munmap(text, fileInfo.st_size);
    munmap(first, numberOfStrings*sizeof(struct str));
    munmap(third, fileInfo.st_size*2 + numberOfStrings*64);
    
    return 0;
}
