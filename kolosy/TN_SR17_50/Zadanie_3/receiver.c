#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main() {
    int val = 0;
    /***********************************
    * odczytaj z potoku nazwanego PIPE zapisana tam wartosc i przypisz ja do zmiennej val
    * posprzataj
    ************************************/
    FILE* file = fopen(PIPE, "r");
    if(file == NULL){
        printf("Can't open PIPE\n");
        return 1;
    }

    fread(&val, sizeof(int), 1, file);

    fclose(file);
    printf("%d square is: %d\n", val, val*val);
    return 0;
}
