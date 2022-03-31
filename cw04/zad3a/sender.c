//
// Created by mateuszskowron on 30.03.2022.
//
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int received_signals = 0;
int is_sigusr2_signal = 0;

void signal1_handler(int sig) {
    received_signals++;
}

void signal2_handler(int signum) {
    is_sigusr2_signal = 1;
}

int main(int argc, char** argv) {
    if(argc < 4) {
        fprintf(stderr, "Arguments number invalid!\n");
        exit(1);
    }

    int pid = atoi(argv[1]);
    int count = atoi(argv[2]);

    sigset_t signal_set;
    sigfillset(&signal_set);

    if(strcmp(argv[3], "SIGRT") == 0) {
        sigdelset(&signal_set, SIGRTMIN);
        sigdelset(&signal_set, SIGRTMIN + 1);
    }
    else {
        sigdelset(&signal_set, SIGUSR1);
        sigdelset(&signal_set, SIGUSR2);
    }

    sigprocmask(SIG_SETMASK, &signal_set, NULL);


    struct sigaction act1;
    act1.sa_handler = signal1_handler;
    act1.sa_flags = 0;
    sigemptyset(&act1.sa_mask);

    // OBSLUGA SYGNALOW
    // SIGRT      -> SIGRTMIN
    // KILL/QUEUE -> SIGUSR1
    if(strcmp(argv[3], "SIGRT") == 0)
    {
        sigaction(SIGRTMIN, &act1, NULL);
    }
    else
    {
        sigaction(SIGUSR1, &act1, NULL);
    }

    if(strcmp(argv[3], "SIGRT") == 0) {
        sigaction(SIGRTMIN, &act1, NULL);
    }
    else {
        sigaction(SIGUSR1, &act1, NULL);
    }


    struct sigaction act2;
    act2.sa_sigaction = signal2_handler;
    act2.sa_flags = SA_SIGINFO;
    sigemptyset(&act2.sa_mask);

    //OBSLUGA SYGNALU OZNACZAJACEGO KONIEC WYSYLANIA SYGNALOW
    //SIGRT      -> SIGRTMIN + 1
    //KILL/QUEUE -> SIGUSR2
    if(strcmp(argv[3], "SIGRT") == 0)
    {
        sigaction(SIGRTMIN + 1, &act2, NULL);
    }
    else
    {
        sigaction(SIGUSR2, &act2, NULL);
    }

    for(int i = 0; i < count; i++) {
        if(strcmp(argv[3], "SIGQUEUE") == 0)
        {
            sigqueue(pid, SIGUSR1, (union sigval) {.sival_int = 0});
        }
        else if (strcmp(argv[3], "SIGRT") == 0)
        {
            kill(pid, SIGRTMIN);
        }
        else if (strcmp(argv[3], "KILL") == 0)
        {
            kill(pid, SIGUSR1);
        }
    }

    if(strcmp(argv[3], "SIGQUEUE") == 0) {
        sigqueue(pid, SIGUSR2, (union sigval){.sival_int = 0});
    }
    else if (strcmp(argv[3], "SIGRT") == 0)
    {
        kill(pid, SIGRTMIN + 1);
    }
    else if (strcmp(argv[3], "KILL") == 0)
    {
        kill(pid, SIGUSR2);
    }

    sigset_t suspend_set;
    sigfillset(&suspend_set);

    if(strcmp(argv[3], "SIGRT") == 0)
    {
        sigdelset(&suspend_set, SIGRTMIN  + 1);
        sigdelset(&suspend_set, SIGRTMIN);
    }
    else
    {
        sigdelset(&suspend_set, SIGUSR2);
        sigdelset(&suspend_set, SIGUSR1);
    }

    //CZEKAM DOPOKI NIE ODBIORE SYGNALU OZNACZAJACEGO KONIEC WYSYLANIA SYGNALOW
    while (is_sigusr2_signal == 0) {
        sigsuspend(&suspend_set);
    }

    printf("[SENDER] Sent %d signals, got back %d\n", count, received_signals);

    return 0;
}
