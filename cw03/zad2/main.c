#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>

#define LENGTH 50

FILE * raportFile;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int main(int argc,char** argv)
{
    if(argc != 3)
    {
        printf("There has to be exactly two arguments: width, number of processes.\n");
        exit(1);
    }

    int length = strlen(argv[1]);
    for (int i = 0; i < length; i++)
    {
        if (!isdigit(argv[1][i]) && argv[1][i]!='.')
        {
            printf("Entered input is not a number\n");
            exit(1);
        }
    }

    length = strlen(argv[2]);
    for (int i = 0; i < length; i++) {
        if (!isdigit(argv[2][i]))
        {
            printf("Entered input is not a number\n");
            exit(1);
        }
    }

    struct tms **tms_time = malloc(2 * sizeof(struct tms *));
    clock_t real_time[6];
    for (int i = 0; i < 2; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms *));
    }
    real_time[0] = times(tms_time[0]);

    double width = atof(argv[1]);
    int n = atoi(argv[2]);
    int current_process = 0;
    int* array = (int *) calloc(n,sizeof(int));

    double middle = width/2.0;
    while (middle < 1.0)
    {
        array[current_process]++;
        current_process = (current_process + 1) % n;

        middle += width;
    }

    pid_t child_pid;

    middle = width/2.0;
    char* p1 = (char *) calloc(LENGTH,sizeof(char));
    char* p2 = (char *) calloc(LENGTH,sizeof(char));
    char* p3 = (char *) calloc(LENGTH,sizeof(char));
    char* p4 = (char *) calloc(LENGTH,sizeof(char));

    for(int i = 0; i < n; i++)
    {

        snprintf(p1, LENGTH, "%f", middle);
        snprintf(p2, LENGTH, "%d", array[i]);
        snprintf(p3, LENGTH, "%f", width);
        snprintf(p4, LENGTH, "%d", i+1);

        char *const parmList[] = {p1, p2, p3,p4,NULL};

        child_pid = fork();
        if(child_pid == 0 ) {
            execv("./child", parmList);
        }
        wait(NULL);
        middle = middle + (width * array[i]);

        memset(p1, 0, LENGTH);
        memset(p2, 0, LENGTH);
        memset(p3, 0, LENGTH);
        memset(p4, 0, LENGTH);
    }
    free(p1);
    free(p2);
    free(p3);
    free(p4);
    free(array);

    float total = 0.0;
    float result = 0.0;
    char* filename = (char *) calloc(LENGTH,sizeof(char));
    char* number = (char *) calloc(LENGTH,sizeof(char));

    for(int i = 1; i <= n; i++)
    {

        sprintf(number, "%d", i);

        strcat(filename,"w");
        strcat(filename,number);
        strcat(filename,".txt");

        FILE * file;
        file = fopen(filename,"r");

        fscanf(file, "%f", &result);
        total += result;

        memset(filename, 0, LENGTH);
        memset(number, 0, LENGTH);

        fclose(file);
    }

    free(filename);
    free(number);

    printf("Wynik: %f\n",total);
    raportFile = fopen("./raport1.txt","a");

    real_time[1] = times(tms_time[1]);

    fprintf(raportFile,"   Real      User      System\n");
    fprintf(raportFile,"%lf   ", calculate_time(real_time[0], real_time[1]));
    fprintf(raportFile,"%lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    fprintf(raportFile,"%lf \n", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    fclose(raportFile);

    return 0;
}