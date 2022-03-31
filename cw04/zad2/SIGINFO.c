//
// Created by mateuszskowron on 30.03.2022.
//
#include <stdio.h>
#include <signal.h>

void signal_handler(int sig, siginfo_t *info, void *context)
{
    printf("Signal number: %d\n", info->si_signo);
    printf("Sending process ID: %d\n", info->si_pid);
    printf("Real user ID of sending process: %d\n", info->si_uid);
    printf("User time consumed: %ld\n", info->si_utime);
    printf("System time consumed: %ld\n", info->si_stime);
}

int main(int argc, char ** argv)
{
    struct sigaction act;

    act.sa_sigaction = signal_handler;
    act.sa_flags = SA_SIGINFO;

    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);

    raise(SIGUSR1);
}

