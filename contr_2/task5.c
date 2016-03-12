#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread/pthread.h>
#include <dirent.h>

int cs_work(const char *pathname){
    struct dirent *dirEntry;
    DIR *dir;
    dir = opendir(pathname);
    dirEntry = readdir(dir);
    int counter = 0;
    while ((dirEntry = readdir(dir))){
        char p[1<<10];
        int strLength = snprintf(p, sizeof(p) - 1, "%s/%s", pathname, dirEntry->d_name);
        p[strLength] = 0;
        if (dirEntry->d_type == DT_DIR){
            if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
                continue;
            counter += cs_work(p);
        }else if (dirEntry->d_type == DT_LNK || dirEntry->d_type == DT_REG){
            if (strncmp(dirEntry->d_name, ".", 1) == 0)
                continue;
            FILE *fd;
            fd = fopen(p, "r");
            fseek(fd, 0L, SEEK_END);
            counter += ftell(fd);
            fclose(fd);
        }
    }
    closedir(dir);
    return counter;
}


int co_work(const char *pathname){
    struct dirent *dirEntry;
    DIR *dir;
    dir = opendir(pathname);
    dirEntry = readdir(dir);
    int counter = 0;
    while ((dirEntry = readdir(dir))){
        char p[1<<10];
        int strLength = snprintf(p, sizeof(p)-1, "%s/%s", pathname, dirEntry->d_name);
        p[strLength] = 0;
        if (dirEntry->d_type == DT_DIR){
            if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
                continue;
            counter += co_work(p);
        }
        else if (dirEntry->d_type == DT_LNK || dirEntry->d_type == DT_REG){
            if (strncmp(dirEntry->d_name, ".", 1) == 0)
                continue;
            counter++;
        }
    }
    closedir(dir);
    return counter;
}

void *func(void *vars){
    pthread_mutex_t *mut = vars;
    while (1){
        pthread_mutex_lock(mut);
        int flag = -1;
        char name[1<<7] = {0};
        char tmp[1<<5] = {0};
        if (scanf("%s %s", tmp, name) == EOF){
            pthread_mutex_unlock(mut);
            return NULL;
        }
        
        if (strcmp("co", tmp) == 0){
            flag = 1;
        }else{
            flag = 0;
        }
        pthread_mutex_unlock(mut);
        int answer = -1;
        if (flag == 0){
            answer = cs_work(name);
        }else if (flag == 1){
            answer = co_work(name);
        }
        printf("%s %s %d\n", flag ? "co":"cs", name, answer);
    }
}

int main(int argc, const char * argv[]){
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    int threadsNum = atoi(argv[1]);
    pthread_t **threads = (pthread_t**)calloc(threadsNum, sizeof(pthread_t*));
    pthread_mutex_t *data = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    data = &mutex;
    for (int i = 0; i < threadsNum; i++){
        threads[i] = (pthread_t*)calloc(1, sizeof(pthread_t));
        pthread_create(threads[i], NULL, func, data);
    }
    for (int i = 0; i < threadsNum; i++){
        pthread_join(*threads[i], NULL);
        free(threads[i]);
    }
    return 0;
}
