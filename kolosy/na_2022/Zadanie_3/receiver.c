#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main() {
    int val = 0;

    FILE* file = fopen(PIPE,"r");
    fread(&val,sizeof(int),1,file);
    fclose(file);

    printf("%d square is: %d\n", val, val*val);

    return 0;
}
