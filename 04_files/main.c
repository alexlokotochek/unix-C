#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include "hash.h"

void countWords(FILE* file, char* filepath){
    int counter = 0, flag = 1;
    char c;
    while (1){
        c = getc(file);
        if (c==EOF)
            break;
        if ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c=='_') || (c>='0' && c<='9') ){
            if (flag == 0)
                counter++;
            flag = 1;
        }else
            flag = 0;
    }
    printf("%s: %d\n", filepath, counter+1);
    fclose(file);
}

int C_followSymlinksOnlyOnce = 0, noS_followSymlinks = 1;
struct Table* visited;


void insideDir(const char* changddirname, int depth){

    DIR* dir = NULL;
    struct dirent entry;
    struct dirent *entryPtr = NULL;
    int retval = 0;
    char pathName[255];
    dir = opendir(changddirname);
    if (dir == NULL)
        return;
    retval = readdir_r(dir, &entry, &entryPtr);
    while (entryPtr != NULL){
        if (retval != 0){
            fprintf(stderr, "couldn't open something. error %d", retval);
        }
        struct stat entryInfo;
        strcpy( pathName, changddirname );
        strcat( pathName, "/");
        strcat( pathName, entry.d_name);
        lstat(pathName, &entryInfo);
        if ( strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0){
            retval = readdir_r(dir, &entry, &entryPtr);
            continue;
        }
        if (S_ISDIR(entryInfo.st_mode)){
            if (depth > 1){
                depth--;
                insideDir(pathName, depth);
                ++depth;
            }else if (depth == -1)
                insideDir(pathName, depth);
        }else if (S_ISREG(entryInfo.st_mode)){
                        //file
            
            if (C_followSymlinksOnlyOnce == 1){
                if (contains(pathName, visited) == 0){
                    visited = insert(pathName, visited);
                    FILE* file = fopen(pathName, "r");
                    countWords(file, pathName);
                }
            }
            
        }else if (S_ISLNK(entryInfo.st_mode && noS_followSymlinks == 0)){
                        //symlink
                char buf[255];
                realpath(pathName, buf);
                struct stat entryLinkInfo;
                lstat(buf, &entryLinkInfo);
                if (S_ISDIR(entryLinkInfo.st_mode)){
                depth--;
                insideDir(buf, depth);
                depth++;
            }else if (S_ISREG(entryLinkInfo.st_mode)){
                if (C_followSymlinksOnlyOnce == 1){
                    if (contains(pathName, visited) == 0){
                        visited = insert(pathName, visited);
                        insideDir(buf, depth);
                    }
                }
            }else
                insideDir(buf, depth);
        }
        retval = readdir_r(dir, &entry, &entryPtr);
    }
}

int main(int argc, const char* argv[]) {
    
    visited = createTable(8);
    
    int depth = 0;
    for (int i = 1; i < argc; ++i){
        if (strcmp(argv[i], "-r") == 0){
            depth = atoi(argv[i+1]);
            if (depth == 0)
                depth--;
        }
        if (strcmp(argv[i], "-s")==0)
            noS_followSymlinks = 0;
        if (strcmp(argv[i], "-c")==0){
            noS_followSymlinks = 1;
            C_followSymlinksOnlyOnce = 1;
        }
    }
    
    insideDir(argv[1], depth);
    
    clearTable(visited);

    return 0;
}

