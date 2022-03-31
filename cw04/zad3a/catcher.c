//
// Created by mateuszskowron on 30.03.2022.
//
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int sigusr1_received = 0;
int is_sigusr2_signal = 0;
int sender_pid = 0;

void signal1_handler(int sig) {
    sigusr1_received++;
}

void signal2_handler(int sig, siginfo_t *info, void *context) {
    is_sigusr2_signal = 1;
    sender_pid = info->si_pid;
}

int main(int argc, char** argv)
{
    if(argc != 2) {
        fprintf(stderr, "Wrong number of arguments!\n");
        exit(1);
    }

    sigset_t signal_set;
    sigfillset(&signal_set);

    if(strcmp(argv[1], "SIGRT") == 0)
    {
        // Catcher przyjmuje tylko SIGRTMIN oraz SIGRTMIN +1.
        sigdelset(&signal_set, SIGRTMIN);
        sigdelset(&signal_set, SIGRTMIN + 1);
    }
    else
    {
        // Catcher przyjmuje tylko SIGUSR1 oraz SIGUSR2.
        sigdelset(&signal_set, SIGUSR1);
        sigdelset(&signal_set, SIGUSR2);
    }

    //Ustawiamy maskę
    sigprocmask(SIG_SETMASK, &signal_set, NULL);

    struct sigaction act1;
    act1.sa_handler = signal1_handler;
    act1.sa_flags = 0;
    sigemptyset(&act1.sa_mask);

    // OBSLUGA SYGNALOW
    // SIGRT      -> SIGRTMIN
    // KILL/QUEUE -> SIGUSR1
    if(strcmp(argv[1], "SIGRT") == 0)
    {
        sigaction(SIGRTMIN, &act1, NULL);
    }
    else
    {
        sigaction(SIGUSR1, &act1, NULL);
    }

    struct sigaction act2;
    act2.sa_sigaction = signal2_handler;
    act2.sa_flags = SA_SIGINFO;
    sigemptyset(&act2.sa_mask);

    //OBSLUGA SYGNALU OZNACZAJACEGO KONIEC WYSYLANIA SYGNALOW
    //SIGRT      -> SIGRTMIN + 1
    //KILL/QUEUE -> SIGUSR2
    if(strcmp(argv[1], "SIGRT") == 0)
    {
        sigaction(SIGRTMIN + 1, &act2, NULL);
    }
    else
    {
        sigaction(SIGUSR2, &act2, NULL);
    }

    printf("[CATCHER] Catcher PID: %d\n", getpid());

    sigset_t suspend_set;
    sigfillset(&suspend_set);

    //SYGNAL OZNACZAJACEGO KONIEC WYSYLANIA SYGNALOW
    if(strcmp(argv[1], "SIGRT") == 0)
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

    printf("[CATCHER] Received %d signals\n", sigusr1_received);

    //ODSYLAM TYLE SAMO SYGNALOW ILE ODEBRALEM
    for(int i = 0; i < sigusr1_received; i++) {
        if(strcmp(argv[1], "SIGQUEUE") == 0)
        {
            sigqueue(sender_pid, SIGUSR1, (union sigval){.sival_int = i});
        }
        else if (strcmp(argv[1], "SIGRT") == 0)
        {
            kill(sender_pid, SIGRTMIN);
        }
        else if(strcmp(argv[1], "KILL") == 0)
        {
            kill(sender_pid, SIGUSR1);
        }
    }

    //WYSYLAM SYGNAL OZNACZAJACY KONIEC WYSYLANIA SYGNALOW
    if(strcmp(argv[1], "SIGQUEUE") == 0)
    {
        sigqueue(sender_pid, SIGUSR2, (union sigval){.sival_int = 0});
    }
    else if(strcmp(argv[1], "SIGRT") == 0)
    {
        kill(sender_pid, SIGRTMIN + 1);
    }
    else if (strcmp(argv[1], "KILL") == 0)
    {
        kill(sender_pid, SIGUSR2);
    }

    return 0;
}
