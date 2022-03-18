#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/times.h>
#include "functions.h"

FILE * raportFile;

void copy(FILE * fileRead,FILE * fileWriteTo)
{
    const size_t lineSize = 256;
    char * buffor = malloc(sizeof(char)*lineSize);

    char c;
    int i = 0;
    int flag;
    int j;

    while (fread(&c,sizeof(char),1,fileRead) == 1)
    {
        if(c == '\n'){
            buffor[i] = '\n';
            j = 0;
            flag = 0;
            while(buffor[j]!='\n')
            {
                if(isspace(buffor[j]) == 0)
                {
                    flag = 1;
                    break;
                }
                j += 1;
            }
            if(flag == 1)
            {
                fwrite(buffor,sizeof(char),i+1,fileWriteTo);
            }
            i = 0;
            continue;
        }
        buffor[i] = c;
        i += 1;
    }

    free(buffor);
}

int main(int argc, char** argv) {
    raportFile = fopen("./pomiar_zad_1.txt","a");

    struct tms **tms_time = malloc(2 * sizeof(struct tms *));
    clock_t real_time[6];
    for (int i = 0; i < 2; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms *));
    }

    printf("   Real      User      System\n");
    fprintf(raportFile,"   Real      User      System\n");

    real_time[0] = times(tms_time[0]);


    char ** filePaths = getFiles(argc,argv);
    FILE * fileRead = fopen(filePaths[0],"r");
    FILE * fileWriteTo = fopen(filePaths[1],"w");

    if(fileRead == NULL)
    {
        printf("File %s was not found or can't be accessed\n", filePaths[0]);
    }
    else if (fileWriteTo == NULL)
    {
        printf("File %s was not found or can't be accessed\n", filePaths[1]);
    }
    else
    {
        copy(fileRead,fileWriteTo);
        fclose(fileRead);
        fclose(fileWriteTo);
    }

    if(argc > 3){
        free(filePaths[0]); free(filePaths[1]);
    }

    free(filePaths);

    real_time[1] = times(tms_time[1]);
    printf("%lf   ", calculate_time(real_time[0], real_time[1]));
    printf("%lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    printf("%lf ", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
    printf("\n");

    fprintf(raportFile,"%lf   ", calculate_time(real_time[0], real_time[1]));
    fprintf(raportFile,"%lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    fprintf(raportFile,"%lf \n", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    fclose(raportFile);
    return 0;
}
