//
// Created by mateuszskowron on 31.03.2022.
//
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int sigusr1_received = 0;
int sender_pid = 0;
int got_sig2 = 0;
char* send_mode;

// ODEBRANIE:
// SIGUSR1  dla kill lub queue --> odsyłam SIGUSR1
// SIGRTMIN dla sigrt --> odsyłam SIGRTMIN
void sigusr1(int signum, siginfo_t *info, void *context) {
    sender_pid = info->si_pid;
    sigusr1_received++;

    if(strcmp(send_mode, "SIGQUEUE") == 0) {
        sigqueue(sender_pid, SIGUSR1, (union sigval){.sival_int = sigusr1_received - 1});
    }
    else if(strcmp(send_mode, "SIGRT") == 0) {
        kill(sender_pid, SIGRTMIN);
    }
    else if(strcmp(send_mode,"KILL") == 0) {
        kill(sender_pid, SIGUSR1);
    }
}

// ODEBRANIE:
// SIGUSR2  dla kill lub queue --> odsyłam SIGUSR2
// SIGRTMIN + 1 dla sigrt --> odsyłam SIGRTMIN + 1
// markuje flage, że już odebrałem wszystko.
void sigusr2(int signum, siginfo_t *info, void *context) {

    sender_pid = info->si_pid;
    if(strcmp(send_mode, "SIGQUEUE") == 0) {
        sigqueue(sender_pid, SIGUSR2, (union sigval){.sival_int = 0});
    }
    else if(strcmp(send_mode, "SIGRT") == 0) {
        kill(sender_pid, SIGRTMIN + 1);
    }
    else if(strcmp(send_mode,"KILL") == 0) {
        kill(sender_pid, SIGUSR2);
    }
    got_sig2 = 1;
}


int main(int argc, char** argv) {

    if(argc != 2) {
        fprintf(stderr, "Arguments number invalid!\n");
        exit(1);
    }

    send_mode = malloc(sizeof(char) * (strlen(argv[1]) + 1));
    send_mode = argv[1];

    sigset_t block;
    sigfillset(&block);

    if(strcmp(send_mode, "SIGRT") == 0) {
        sigdelset(&block, SIGRTMIN);
        sigdelset(&block, SIGRTMIN + 1);
    }
    else {
        sigdelset(&block, SIGUSR1);
        sigdelset(&block, SIGUSR2);
    }

    sigprocmask(SIG_SETMASK, &block, NULL);

    // Nasłuchiwanie na:
    // SIGUSR1  dla kill lub queue
    // SIGRTMIN dla sigrt
    struct sigaction action1;
    action1.sa_sigaction = sigusr1;
    action1.sa_flags = SA_SIGINFO;
    sigemptyset(&action1.sa_mask);

    if(strcmp(send_mode, "SIGRT") == 0) {
        sigaction(SIGRTMIN, &action1, NULL);
    }
    else {
        sigaction(SIGUSR1, &action1, NULL);
    }

    // Nasłuchiwanie na:
    // SIGUSR2  dla kill lub queue
    // SIGRTMIN+1 dla sigrt
    struct sigaction action2;
    action2.sa_sigaction = sigusr2;
    action2.sa_flags = SA_SIGINFO;
    sigemptyset(&action2.sa_mask);

    if(strcmp(send_mode, "SIGRT") == 0) {
        sigaction(SIGRTMIN + 1, &action2, NULL);
    }
    else {
        sigaction(SIGUSR2, &action2, NULL);
    }


    printf("[CATCHER] Catcher PID: %d\n", getpid());

    //Dopóki nie otrzymam sygnału SIGUSR2 - czyli wszystkich sygnałów.
    while (got_sig2 == 0) {}

    printf("[CATCHER] Received %d signals\n", sigusr1_received);

    return 0;
}