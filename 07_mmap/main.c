#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define _BSD_SOURCE

double* data;
int columns, rows = -1;

void swap(double* a, double *b){
    double temp = *a;
    *a = *b;
    *b = temp;
}

// система координат: х слева направо, у сверху вниз, нумерация с (1, 1)

void getinfo(){
    printf("rows:%d; columns:%d\n", rows, columns);
}

void set(size_t x, size_t y, double a){
    data[x-1 + (y-1)*columns] = a;
}

void get(size_t x, size_t y){
    printf("(%zu,%zu): %4.2lf\n", x, y, data[x-1 + (y-1)*columns]);
}

void sumRow(size_t y){
    double sum = 0;
    for (size_t i = (y-1)*columns; i < y*columns; ++i)
        sum += data[i];
    printf("sum of row %zu: %4.2lf\n", y, sum);
}

void sumCol(size_t x){
    double sum = 0;
    for (size_t i = x-1; i < (columns)*(rows-1)+x; i+=columns)
        sum += data[i];
    printf("sum of col %zu: %4.2lf\n", x, sum);
}

void swapRows(size_t row1, size_t row2){
    size_t a, b;
    for (size_t i = 0; i < columns; i++){
        a = (row1-1)*columns+i;
        b = (row2-1)*columns+i;
        swap(&data[a], &data[b]);
    }
}

void transpose(){
    double* buf = (double*)malloc(rows*columns*sizeof(double));
    for (size_t y = 0; y < rows; ++y){
        for (size_t x = 0; x < columns; ++x)
            buf[x*rows + y] = data[y*columns + x];
    }
    for (size_t i = 0; i < rows*columns; ++i){
        data[i] = buf[i];
    }
    free(buf);
    int temp = columns;
    columns = rows;
    rows = temp;
}

void fillFileWithDoubles(size_t n, int fd){
    double* buf = (double*)malloc(n*sizeof(double));
    for (int i = 0; i < n; ++i){
        buf[i] = 0.0;
        //printf("%lf ", buf[i]);
    }
    write(fd, (void*)buf, n*sizeof(double));
}

int main(int argc, const char * argv[]) {
    setbuf(stdout, 0);
    const char* path = argv[1];
    int fd = open(path, O_RDWR | O_CREAT, 0666);
    if (fd < 0){
        printf("error opening or creating file\n");
        exit(1);
    }
    
// для тестов: указать размер матрицы, файл заполнится дабловыми нулями
//    fillFileWithDoubles(12, fd);

    // argv[1..3]: путь; колонки; строчки
    if (argc > 4 || argc <= 1){
        printf("wrong arguments\n");
        exit(1);
    }
    
    columns = atoi(argv[2]);
    if (argc == 4)
        rows = atoi(argv[3]);
    
    struct stat fileInfo;
    lstat(path, &fileInfo);
    int elementsNumber = (int)fileInfo.st_size/sizeof(double);
    if (rows == -1){
        rows = elementsNumber/columns;
        ftruncate(fd, columns*rows*sizeof(double));
    }
    
    data = (double*)mmap(NULL, fileInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    char* command = (char*)malloc(255*sizeof(char));
    int x, y, n;
    printf("print \"exit\" to quit\n");
    while(1){
        scanf("%s", command);
        if (strcmp(command, "exit") == 0)
            break;
        if (strcmp(command, "transpose") == 0){
            transpose();
            continue;
        }
        if (strcmp(command, "sum")==0){
            scanf("%s%d", command, &y);
            if (strcmp(command, "row")==0){
                sumRow(y);
                continue;
            }
            sumCol(y);
            continue;
        }
        if (strcmp(command, "getinfo")==0){
            getinfo();
            continue;
        }
        scanf("%d%d", &x, &y);
        if (strcmp(command, "get")==0){
            get(x, y);
            continue;
        }
        if (strcmp(command, "set")==0){
            scanf("%d", &n);
            set(x, y, n);
            continue;
        }
        if (strcmp(command, "swap")==0){
            swapRows(x, y);
            continue;
        }
        printf("wrong command\n");
    }
    
    msync(data, fileInfo.st_size, MS_SYNC);
    munmap(data, fileInfo.st_size);
    close(fd);
    free(command);
    
    return 0;
}
