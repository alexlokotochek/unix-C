#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, const char * argv[]) {
//    char s[512];
//    const char* input = argv[1];
//    const char* output = argv[2];
//    while(1){
//        scanf("%s", s);
//    }
//    scanf("%s", s);
//    for (int i = 0; i < strlen(input); ++i){
//        char* currentSearch = (char*)malloc(sizeof(char));
//        currentSearch[0] = input[i];
//        char* search = strstr(s, currentSearch);
//        while(search != NULL){
//            search[0] = output[i];
//            search = strstr(s, currentSearch);
//        }
//    }
    
    
    const char* input = "abc";
    const char* output = "cde";
    char c, comp;
    size_t n = strlen(input);
    while(1){
        scanf("%c", &c);
        for (size_t i = 0; i < n; ++i){
            comp = input[i];
            if (c == comp){
                printf("%c", output[i]);
                break;
            }
            if (i == n-1)
                printf("%c", c);
        }
    }
    
    //printf("\n%s",s);
    
    return 0;
}
