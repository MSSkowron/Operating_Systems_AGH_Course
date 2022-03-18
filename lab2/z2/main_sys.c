#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/times.h>
#include "functions.h"

FILE * raportFile;

void check(int file,char* c,int* charCounter,int* rowCounter)
{
    const size_t lineSize = 256;
    char * buffor = malloc(sizeof(char)*lineSize);

    char ch;
    int i = 0;
    int flag;
    int j;

    while (read(file,&ch,1) == 1)
    {
        if(ch == '\n') {
            buffor[i] = '\n';
            j = 0;
            flag = 0;
            while (buffor[j] != '\n')
            {
                if(buffor[j] == c[0])
                {
                    *charCounter += 1;
                    flag = 1;
                }
                j += 1;
            }
            if (flag == 1)
            {
                *rowCounter += 1;
            }
            i = 0;
            continue;
        }
        buffor[i] = ch;
        i += 1;
    }
    free(buffor);
}

int main(int argc, char** argv) {
    if(argc != 3)
    {
        printf("Not enough argument!\n");
        return 1;
    }
    if(strlen(argv[1])!=1)
    {
        printf("First argument need to be a character!\n");
        return 1;
    }
    raportFile = fopen("./pomiar_zad_2.txt","a");

    struct tms **tms_time = malloc(2 * sizeof(struct tms *));
    clock_t real_time[6];
    for (int i = 0; i < 2; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms *));
    }

    real_time[0] = times(tms_time[0]);

    int file = open(argv[2],O_RDONLY);

    if(file == -1)
    {
        printf("File %s was not found or can't be accessed\n",argv[2]);
    }
    else
    {
        int charCounter = 0;
        int rowCounter = 0;
        int* cc = &charCounter;
        int* rc = &rowCounter;
        check(file,argv[1],cc,rc);
        printf("Char counter: %d\n",charCounter);
        printf("Row counter: %d\n",rowCounter);
        close(file);
    }

    real_time[1] = times(tms_time[1]);

    fprintf(raportFile,"   Real      User      System\n");
    fprintf(raportFile,"%lf   ", calculate_time(real_time[0], real_time[1]));
    fprintf(raportFile,"%lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    fprintf(raportFile,"%lf \n", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    fclose(raportFile);
    return 0;
}
