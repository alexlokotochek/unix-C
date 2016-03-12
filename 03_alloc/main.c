#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define cellSize 1024

struct memoryCell{
    struct memoryCell* next;
    struct memoryCell* prev;
    size_t size;
    size_t isFree;
};

struct memoryCell* firstCell;

void firstAlloc(){
    firstCell = (struct memoryCell*)sbrk(cellSize);
    firstCell->size = 1024 - sizeof(struct memoryCell);
    firstCell->isFree = 1;
    firstCell->prev = NULL;
    firstCell->next = NULL;
}

struct memoryCell* getLast(){
    struct memoryCell* lastCell = firstCell;
    while (lastCell->next)
        lastCell = lastCell->next;
    return lastCell;
}

void* my_malloc(ssize_t requestedSize){
    if (!firstCell)
        firstAlloc();
    struct memoryCell* currentCell = firstCell;
    while (currentCell->next)
        if (currentCell->isFree == 1 && currentCell->size >= requestedSize){
            currentCell->isFree = 0;
            return ((void*)currentCell + sizeof(struct memoryCell));
        }
    //current cell не нашла свободных подходящего размера и дошла до последней
    //значит надо выделить новую клетку
    struct memoryCell* returnCell = (struct memoryCell*)sbrk(cellSize);
    // эта ячейка вернется, если просят меньше cellSize; иначе расширяем её
    returnCell->size = cellSize - sizeof(struct memoryCell);
    returnCell->prev = currentCell;
    returnCell->next = NULL;
    returnCell->isFree = 0;
    requestedSize -= returnCell->size;
    //наращиваем последнюю клетку
    while (requestedSize > 0){
        returnCell->size += cellSize;
        returnCell = (struct memoryCell*) ( (void*)sbrk(cellSize) - 1024);
        requestedSize -= cellSize;
    }
    return ( (void*)returnCell + sizeof(struct memoryCell) );
}

void my_free(void* cell){
    struct memoryCell* cellToFree = (struct memoryCell*)(cell - sizeof(struct memoryCell));
    cellToFree->isFree = 1;
    if (cellToFree->next){
        if (cellToFree->next->isFree == 1){
            cellToFree->size += cellToFree->next->size;
            if (cellToFree->next->next){
                cellToFree->next->next->prev = cellToFree;
                cellToFree->next = cellToFree->next->next;
            }else
                cellToFree->next = NULL;
        }
    }
    if (cellToFree->prev){
        if (cellToFree->prev->isFree == 1){
            cellToFree->prev->size += cellToFree->size;
            if (cellToFree->next){
                cellToFree->next->prev = cellToFree->prev;
                cellToFree->prev->next = cellToFree->next;
            }else
                cellToFree->prev->next = NULL;
        }
    }
    return;
}

int main(int argc, char** argv){
    setbuf(stdout, NULL);
    char* checkString1 = (char*)my_malloc(255);
    strcpy(checkString1, "hello world1;\n");
    
    printf("%s", checkString1);
    
    my_free(checkString1);
    
    char* checkString2 = (char*)my_malloc(255);
    strcpy(checkString2, "hello world2;\n");
    char* checkString3 = (char*)my_malloc(255);
    strcpy(checkString3, "hello world3;\n");
    
    printf("%s", checkString2);
    
    char* checkString4 = (char*)my_malloc(2049);
    strcpy(checkString4, "hello world4;\n");
    char* checkString5 = (char*)my_malloc(255);
    strcpy(checkString5, "hello world5;\n");
    
    printf("%s", checkString4);
    
    my_free(checkString4);
    my_free(checkString2);
    
    char* checkString6 = (char*)my_malloc(1025);
    strcpy(checkString6, "hello world6;\n");
    printf("%s", checkString3);
    
    printf("%s", checkString6);
    
    my_free(checkString6);
    
    printf("%s", checkString5);
    
    my_free(checkString3);

    printf("%s", checkString5);
    //1243655
    return 0;
}