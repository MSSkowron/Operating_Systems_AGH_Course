#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int lpar, char *tab[]) 
{
    int result;

    // 1) utworzyc potok nazwany 'potok1'
    // 2) wyswietlac liczby otrzymywane z potoku

    mkfifo("potok1",0777);

    FILE* fifo = fopen("potok1","r");

    while (1) {
        fread(&result, sizeof(int), 1, fifo);
        printf("Otrzymano: %d\n", result);
        fflush(stdout);
    }

    fclose(fifo);

    return 0;
}