#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>

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

    for(int i = 0; i < n; i++)
    {

        char* p1 = (char *) calloc(50,sizeof(char));
        char* p2 = (char *) calloc(50,sizeof(char));
        char* p3 = (char *) calloc(50,sizeof(char));
        char* p4 = (char *) calloc(50,sizeof(char));
        snprintf(p1, 50, "%f", middle);
        snprintf(p2, 50, "%d", array[i]);
        snprintf(p3, 50, "%f", width);
        snprintf(p4, 50, "%d", i+1);

        char *const parmList[] = {p1, p2, p3,p4,NULL};

        child_pid = fork();
        wait(NULL);
        if(child_pid == 0 ) {
            execv("./child", parmList);
        }

        free(p1);
        free(p2);
        free(p3);
        free(p4);

        middle = middle + (width * array[i]);
    }

    free(array);

    float total = 0.0;
    float result = 0.0;

    for(int i = 1; i <= n; i++)
    {
        char* filename = (char *) calloc(50,sizeof(char));
        char* number = (char *) calloc(50,sizeof(char));

        sprintf(number, "%d", i);

        strcat(filename,"w");
        strcat(filename,number);
        strcat(filename,".txt");

        FILE * file;
        file = fopen(filename,"r");

        fscanf(file, "%f", &result);
        total += result;

        free(filename);
        free(number);

        fclose(file);
    }

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