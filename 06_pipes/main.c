#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 512
#define _BSD_SOURCE

extern char** environ;

int main(int argc, char* argv[]) {
    setbuf(stdout, 0);
    ///то, что делает баш<
    int input_pipe;
    if((input_pipe = open(getenv("FIFONAME"), O_RDONLY)) < 0){
        printf("error opening fifo\n");
        exit(1);
    }
    //>
    
    int bashPipe[2];// общение баша с программой
    pipe(bashPipe);
    fcntl(bashPipe[0], F_SETFL, O_NONBLOCK);
    fcntl(bashPipe[1], F_SETFL, O_NONBLOCK);
    
    int outputPipe[2];
    pipe(outputPipe);
    fcntl(outputPipe[0], F_SETFL, O_NONBLOCK);
    fcntl(outputPipe[1], F_SETFL, O_NONBLOCK);
    
    int ansPid = fork();
    if (ansPid == 0){
    
        char* currentURL = (char*)malloc(256*sizeof(char)+2); // текущий считываемый url
        char* args[] = {"/usr/bin/curl", currentURL, "--silent", NULL};
        char* c = (char*)malloc(2*sizeof(char));
        char* parsing = (char*)malloc(sizeof(char)*2);
        strcpy(parsing, "");
        
        char* buf = (char*)malloc((BUFSIZE + 2)*sizeof(char));
        char* urls = (char*)malloc(2*sizeof(char));
        char* bashInp = (char*)malloc(2*sizeof(char));
        
        strcpy(urls, "");
        size_t currentAlloc = 2;
        while (read(input_pipe, buf, BUFSIZE) > 0){
            currentAlloc += BUFSIZE;
            urls = realloc(urls, currentAlloc);
            strcat(urls, buf);
        }
        currentAlloc = 2*sizeof(char);
        char* token;
        token = strtok(urls, "\n");
        while (1){
            if (token == NULL)
                break;
            strcpy(currentURL, token);
            pid_t pid = fork();
            if (pid == 0){
                argv[1] = currentURL;
                if (dup2(bashPipe[1], STDOUT_FILENO) < 0){ // перенаправление стандартного вывода в программу
                    printf("error dupling stdout");
                    exit(1);
                }
                execv(args[0], args);
                exit(1);
            }
            else{
                while(wait(0) > 0);
                
                strcpy(bashInp, "");
                while(read(bashPipe[0], buf, BUFSIZE) > 0){
                    currentAlloc += BUFSIZE;
                    bashInp = realloc(bashInp, currentAlloc);
                    strcat(bashInp, buf);
                }
                size_t i = 0;
                while (bashInp != NULL){
                    bashInp = strstr(bashInp, "<a href=\"");
                    if (bashInp == NULL)
                        break;
                    bashInp += strlen("<a href=\"");
                    if (strncmp(bashInp, "http", strlen("http")) == 0){
                        while ((bashInp[i] != '"')){
                            write(outputPipe[1], &(bashInp[i]), sizeof(char));
                            ++i;
                            if (i == strlen(bashInp))
                                break;
                        }
                        write(outputPipe[1], "\n", sizeof(char));
                        bashInp += i;
                        i = 0;
                    }
                }
                bashInp = realloc(bashInp, 2);
                currentAlloc = 2;
            }
            token = strtok(NULL, "\n");
        }
        free(c);
        free(parsing);
        free(token);
        free(currentURL);
        free(urls);
        free(buf);
    }else{
        char* output = (char*)malloc(sizeof(char)*(BUFSIZE + 1));
        while (wait(0) > 0);
        while (read(outputPipe[0], output, BUFSIZE) > 0){
            printf("%s", output);
        }
    }
    return 0;
}