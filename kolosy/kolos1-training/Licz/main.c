#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>


/* 1) wywolaj funkcje 'licz' dla parametrow od 1 do 20. Kazde wywolanie tej
   funkcji ma byc w innym procesie potomnym
   2) proces macierzysty ma wyswietlic pid wszystkich procesow potomnych i
   czekac na zakonczenie ich wszystkich*/

void licz(int x) {
    printf("dla x=%d wynik x2 to %d w procesie o PID: %d\n", x, x * x, getpid());
    fflush(stdout);
    exit(0);
}


int main(int argc, char** argv)
{
    pid_t child;

    for(int i = 1; i <= 20; i++)
    {
        child = fork();
        if(child == -1)
        {
            perror("Error while forking\n");
            exit(1);
        }
        else if(child == 0)
        {
            // Child process
            licz(i);
            break;
        }
        else 
        {
            // Parent process
            // Does nothing here
        }
    }

    for(int i = 1; i <= 20; i++)
    {
        wait(NULL);
    }

    return 0;
}