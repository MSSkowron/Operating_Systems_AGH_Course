#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv){

    if (strcmp("pending", argv[1]) != 0){
        raise(SIGUSR1);
    }

    sigset_t mask;

    if (strcmp("pending", argv[1]) == 0){
        sigpending(&mask);

        if(sigismember(&mask, SIGUSR1)){
            printf("Sygnal oczekuje w dziecku.\n\n");
        }
        else{
            printf("Sygnal nie oczekuje w dziecku.\n\n");
        }
    }
    else if (strcmp("mask", argv[1]) == 0){
        sigpending(&mask);

        if(sigismember(&mask, SIGUSR1)){
            printf("Dziecko odziedziczylo maske.\n");
        }
        else{
            printf("Dziecko nie odziedziczylo maski.\n");
        }
    }

    return 0;
}