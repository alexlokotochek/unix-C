#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[]) {

    if (strcmp(argv[1], "-mj")==0){
        //multijoin
        printf("mj mode\n");
        FILE* pipes[2];
        pipes[0] = popen(argv[2], "r");
        pipes[1] = popen(argv[3], "r");
        char* value_first = (char*)malloc(1<<10);
        char* value_second = (char*)malloc(1<<10);
        char* key_first = (char*)malloc(1<<10);
        char* key_second = (char*)malloc(1<<10);
        int closed[2] = {0, 0};
        fscanf(pipes[0], "%s\t%s\n", key_first, value_first);
        fscanf(pipes[1], "%s\t%s\n", key_second, value_second);

        while(!feof(pipes[0]) && !feof(pipes[1])){
            while (strcmp(key_first, key_second)<0 && !feof(pipes[0])){
                printf("%s\n", value_first);
                fscanf(pipes[0], "%s\t%s\n", key_first, value_first);
            }
            if (strcmp(key_first, key_second)==0){
                printf("%s\t%s\n", value_first, value_second);
                fscanf(pipes[0], "%s\t%s\n", key_first, value_first);
                fscanf(pipes[1], "%s\t%s\n", key_second, value_second);
            }else{
                while (strcmp(key_first, key_second)>0 && !feof(pipes[1])){
                    printf("%s\n", value_second);
                    fscanf(pipes[1], "%s\t%s\n", key_second, value_second);
                }
            }
            closed[0] = feof(pipes[0]);
            closed[1] = feof(pipes[1]);
        }
        
        if (closed[1] != 0){
            printf("%s\n", value_second);
            while (!feof(pipes[0])){
                fscanf(pipes[0], "%s\t%s\n", key_first, value_first);
                printf("%s\n", value_first);
            }
        }
        if (closed[0] != 0){
            printf("%s\n", value_first);
            while (!feof(pipes[1])){
                fscanf(pipes[1], "%s\t%s\n", key_second, value_second);
                printf("%s\n", value_second);
            }
        }
        free(value_first);
        free(value_second);
        free(key_first);
        free(key_second);
        pclose(pipes[0]);
        pclose(pipes[1]);
        
    }else{
        //simple merge
        FILE* pipes[argc-1];
        int closed = 0;
        int opened[argc-1];
        for (int i = 1; i < argc; ++i){
            pipes[i] = popen(argv[i], "r");
            opened[i] = 1;
        }
        
        char* input = (char*)malloc(1<<10);
        while(1){
            for (int i = 1; i < argc; ++i){
                if (opened[i] == 1){
                    if (feof(pipes[i])){
                        opened[i] = 0;
                        ++closed;
                        continue;
                    }
                    fscanf(pipes[i], "%s\n", input);
                    printf("%s\t", input);
                }
            }
            printf("\n");
            if (closed == argc-1)
                break;
        }

        for (int i = 0; i < argc; ++i)
            pclose(pipes[i]);
        free(input);
    }
    return 0;
}
