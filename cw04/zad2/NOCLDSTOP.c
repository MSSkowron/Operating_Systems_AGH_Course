//
// Created by mateuszskowron on 30.03.2022.
//
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

void signal_handler(int sig) {
    printf("Received a signal %d, PID: %d, PPID: %d\n", sig ,getpid(), getppid());
}

int main() {
    struct sigaction act;
    act.sa_handler = &signal_handler;
    act.sa_flags = SA_NOCLDSTOP;

    sigaction(SIGCHLD, &act, NULL);

    int pid = fork();
    if (pid == -1)
    {
        return -1;
    }
    else if (pid == 0)
    {
        while(1) {}
    }
    else
    {
        kill(pid, SIGSTOP);
        sleep(1);
        kill(pid, SIGCONT);
        kill(pid, SIGKILL);
    }
    wait(NULL);

    /*
     * Jeśli dodamy flagę nie obsłużymy sygnałów SIGSTOP oraz SIGCONT,
     * a jedynie SIGKILL.
     */
}