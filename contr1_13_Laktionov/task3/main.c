#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void countWords2(int readPipe){
    char* filename;
    int counter = 0;
    while(1){
        int counter;
        ssize_t readCode = read(readPipe, &filename, sizeof(filename));
        if (readCode == EOF)
            return;
        FILE *currentFile = fopen(filename, "r");

        char* input;
        counter = 0;
        while(!feof(currentFile)){
            fscanf(currentFile,"%s", input);
            ++counter;
        }
        
        printf("\nfilename %s:\t%d\n", filename, counter);
    }
    
}

void countWords1(int readFrom)
{
    while (1){
        char *input;
        ssize_t isEnd = read(readFrom, &input, sizeof(input));
        
        if (isEnd == EOF)
            return;
        
        FILE *currentFile = fopen(input, "r");
        if (currentFile != NULL)
        {
            int words = 0;
            char inputChar;
            while (!feof(currentFile)){
                inputChar = fgetc(currentFile);
                if (inputChar == ' ')
                    words++;
            }
            printf("file: %s; words: %d", input, words);
        }
    }
}


int main(int argc, char *argv[]){

    int processes = atoi(argv[argc-1]);
    int taskpipe[2];

    pipe(taskpipe);
    
    for (int i = 1; i < argc - 2; ++i)
    {
        write(taskpipe[1], &argv[i], sizeof(char**));
    }

    int processID;
    
    for (int i = 0; i < processes; ++i){
        processID = fork();
        if (processID == 0){//если я - ребенок, то начинаю работу с файлами из пайпа для входа
            countWords(taskpipe[0]);//не работает ни countWords1, ни countWords2
        }
    }
    
    return 0;
    
}

