#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char** argv)
{
    if(argc != 2)
    {
        printf("There has to be exactly one argument which is number of processes to run!.\n");
        exit(1);
    }
    int length = strlen(argv[1]);
    for (int i = 0; i < length; i++)
        if (!isdigit(argv[1][i]))
        {
            printf ("Entered input is not a number\n");
            exit(1);
        }

    pid_t child_pid;
    printf("\nPID glownego programu: %d\n", (int)getpid());

    int range = atoi(argv[1]);

    for (int i = 0; i < range; i++)
    {
        child_pid = fork();
        wait(NULL);
        if(child_pid !=0 ) {
            printf("\nTen napis zostal wyswietlony w programie 'main'!\n");
        }
        else
        {
            execvp("./child", NULL);
        }
    }

    return 0;
}