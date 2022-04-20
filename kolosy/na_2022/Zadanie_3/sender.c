#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main(int argc, char* argv[]) {
    if(argc !=2){
        printf("Not a suitable number of program parameters\n");
        return(1);
    }

    mkfifo(PIPE,0666);

    int number = atoi(argv[1]);

    FILE* file = fopen(PIPE,"w");
    fwrite(&number,sizeof(int),1,file);
    fclose(file);

    return 0;
}
