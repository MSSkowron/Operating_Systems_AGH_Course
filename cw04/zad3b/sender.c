//
// Created by mateuszskowron on 31.03.2022.
//
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>


int received_signals = 0;  //liczba otrzymanych sygnalow
int is_sigusr2_signal = 0; //potwierdzenie, ze catcher otrzymal koncowy sygnal

//OTRZYMANIE SYGNALU SIGUSR1/SIGRTMIN
void sigusr1(int signum) {
    received_signals++;
}

//OTRZYMANIE SYGNALU SIGUSR1 W TRYBIE QUEUE, ODSYLAM TO SAMO
void sigusr1_queue(int signum, siginfo_t *info, void *context) {
    received_signals++;
}

//OTRZYMANIE SYGNALU POTWIERDZAJACEGO, ZE CATCHER OTRZYMAL SYGNAL O ZAKOCZENIU PRZESYLANIA SYGNALOW
void sigusr2(int signum) {
    is_sigusr2_signal = 1;
}

int main(int argc, char** argv) {

    if(argc < 4) {
        printf("%d\n", argc);
        printf("%s\n", argv[1]);
        fprintf(stderr, "Arguments number invalidd!\n");
        exit(1);
    }


    int pid = atoi(argv[1]);   //catcher PID
    int count = atoi(argv[2]); //liczba sygnalow
    char* send_mode = malloc(sizeof(char) * (strlen(argv[3]) + 1));
    send_mode = argv[3]; //tryb wysylania

    sigset_t block;
    sigfillset(&block);

    //NIE BLOKUJE SIGRTMIN ORAZ SIGRTMIN + 1
    if(strcmp(send_mode, "SIGRT") == 0) {
        sigdelset(&block, SIGRTMIN);
        sigdelset(&block, SIGRTMIN + 1);
    }
    //NIE BLOKUJE SIGUSR1 ORAZ SIGUSR2
    else {
        sigdelset(&block, SIGUSR1);
        sigdelset(&block, SIGUSR2);
    }

    //USTAWIENIE MASKI
    sigprocmask(SIG_SETMASK, &block, NULL);

    //HANDLER NA SIGUSR1/SIGRTMIN
    struct sigaction action1;
    if(strcmp(send_mode, "SIGQUEUE") == 0)
    {
        action1.sa_sigaction = sigusr1_queue;
    }
    else
    {
        action1.sa_handler = sigusr1;
    }
    action1.sa_flags = SA_SIGINFO;
    sigemptyset(&action1.sa_mask);

    // NASŁUCHUJE - potwierdzenie odebrania sygnału.
    // SIGRT      -> nasłuchuje na SIGRTMIN
    // KILL/QUEUE -> nasłuchuje na SIGUSR1
    if(strcmp(send_mode, "SIGRT") == 0)
    {
        sigaction(SIGRTMIN, &action1, NULL);
    }
    else
    {
        sigaction(SIGUSR1, &action1, NULL);
    }

    // NASŁUCHUJE  - potwierdzenie odebrania ostatniego sygnału.
    // SIGRT      -> SIGRTMIN + 1
    // KILL/QUEUE -> SIGUSR2
    struct sigaction action2;
    action2.sa_handler = sigusr2;
    action2.sa_flags = SA_SIGINFO;
    sigemptyset(&action2.sa_mask);

    if(strcmp(send_mode, "SIGRT") == 0)
    {
        sigaction(SIGRTMIN + 1, &action2, NULL);
    }
    else
    {
        sigaction(SIGUSR2, &action2, NULL);
    }


    // WYSYŁANIE SYGNALOW DO CATCHERA
    for(int i = 0; i < count; i++) {
        if(strcmp(send_mode, "SIGQUEUE") == 0)
        {
            sigqueue(pid, SIGUSR1, (union sigval) {.sival_int = 0});
        }
        else if(strcmp(send_mode,"SIGRT") == 0)
        {
            kill(pid, SIGRTMIN);
        }
        else if(strcmp(send_mode,"KILL") == 0)
        {
            kill(pid, SIGUSR1);
        }
        // OCZEKIWANIE NA ODEBRANIE SYGNALU - CZYLI, AZ NAM CATCHER ODESLE TEN SAM SYGNAL.
        while(received_signals - 1 < i) {}
    }

    //WYSYLAM SYGNAL, ZE ZAKONCZYLEM WYSYLANIE SYGNALOW
    if(strcmp(send_mode, "SIGQUEUE") == 0)
    {
        sigqueue(pid, SIGUSR2, (union sigval){.sival_int = 0});
    }
    else if(strcmp(send_mode,"SIGRT") == 0)
    {
        kill(pid, SIGRTMIN + 1);
    }
    else if(strcmp(send_mode,"KILL") == 0)
    {
        kill(pid, SIGUSR2);
    }

    //CZEKAM, AZ CATCHER ODPOWIE, ZE OTRZYMAL SYGNAL O ZAKONCZENIU WYSYLANIA SYGNALOW.
    while (is_sigusr2_signal == 0) {}

    //PODSUMOWANIE WYSLANYCH/ODEBRANYCH SYGNALOW
    printf("[SENDER] Sent %d signals, received %d signals\n", count, received_signals);

    return 0;
}