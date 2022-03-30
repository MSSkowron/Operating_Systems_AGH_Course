#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>

void signalhandler(int sig){
    printf("Sygnal przechwycony.\n");
}

int main(int argc, char **argv){
    if (argc != 3){
        printf("Zla liczba argumentow. Oczekiwano: ./main {ignore / handler / mask / pending} {fork / exec}\n");
        return -1;
    }

    char option;

    if (strcmp("ignore", argv[1]) == 0){
        option = 'i';
        printf("IGNORE\n\n");
        //ignore SIGUSR1 signal
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp("handler", argv[1]) == 0){
        option = 'h';
        printf("HANDLER\n");
        // handle SIGUSR1 signal with function signalHandle
        signal(SIGUSR1, signalhandler);
    }
    else if (strcmp("mask", argv[1]) == 0 || strcmp("pending", argv[1]) == 0){
        if (strcmp("mask", argv[1]) == 0) {
            option = 'm';
            printf("MASK\n");
        }
        else {
            option = 'p';
            printf("PENDING\n");
        }

        sigset_t mask; // sygnały do blokowania
        sigemptyset (&mask); //wyczyść zbiór blokowanych sygnałów
        sigaddset(&mask, SIGUSR1); //dodaj SIGUSR1 do zbioru

        // Dodaj do zbioru sygnałów zablokowanych
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0){
            perror("Nie udało się zablokować sygnału");
            exit(1);
        }
    }
    else{
        printf("Zly argument: %s\n", argv[1]);
        return -1;
    }

    sigset_t mask;

    // Wysłanie sygnału SIGUSR1 do samego siebie
    raise(SIGUSR1);

    if (option == 'm'){
        sigpending(&mask);

        if (sigismember(&mask, SIGUSR1)){
            printf("Sygnal w masce rodzica.\n");
        }
    }
    else if(option == 'p')
    {
        sigpending(&mask);

        if (sigismember(&mask, SIGUSR1)){
            printf("Sygnal oczekuje w rodzicu.\n");
        }
    }

    // Tworzenie potomka funkcją fork.
    if (strcmp(argv[2], "fork") == 0){
        pid_t pid = fork();

        if (pid == 0){
            if (option != 'p')
            {
                raise(SIGUSR1);
            }

            if (option == 'm'){
                sigpending(&mask);

                if (sigismember(&mask, SIGUSR1)){
                    printf("Dziecko odziedziczylo maske.\n\n");
                }
                else{
                    printf("Dziecko nie odziedziczylo maski.\n\n");
                }
            }
            else if (option == 'p')
            {
                sigpending(&mask);

                if (sigismember(&mask, SIGUSR1)){
                    printf("Sygnal oczekuje w dziecku.\n\n");
                }
                else{
                    printf("Sygnal nie oczekuje w dziecku.\n\n");
                }
            }
        }
    }
    // Użycie exec. Nie rozpatrujemy opcji handler
    else if(strcmp(argv[2], "exec") == 0 && option != 'h'){
        execl("./exec",  "./exec", argv[1], NULL);
    }
    else {
        printf("Zly argument: %s\n", argv[2]);
        return -1;
    }

    wait(0);
    return 0;
}