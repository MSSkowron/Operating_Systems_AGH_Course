#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int i = 0;
FILE* fifo;


void handler(int sig, siginfo_t *info, void *ucontext)
{
    printf("Sending...\n");
    fwrite(&i, sizeof(int), 1, fifo);
}
/*
program przy kazdym uzyciu klawiszy ctrl-c ma wyslac przez potok
nazwany 'potok1' zawarto�� zmiennej 'i' 
*/

int main (int lpar, char *tab[])
{
    struct sigaction action;
    action.sa_handler = handler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    sigaction(SIGINT, &action, NULL);

    fifo = fopen("potok1", "w");

    while(1) 
    {
        printf("%d ",i++); fflush(stdout);
        sleep(1);
    }

    fclose(fifo);

    return 0;
}