#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


void sighandler(int signo, siginfo_t *siginfo, void *context) {
    int value = siginfo->si_value.sival_int;
    if (signo == SIGUSR1) {
        printf("Signal SIGUSR1. Value: %d\n", value);
    } else {
        printf("Signal SIGUSR2. Value: %d\n", value);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    //-- zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal --
    //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci
    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        fprintf(stderr, "Failed to setup handler for SIGUSR1\n");
    }

    if (sigaction(SIGUSR2, &action, NULL) == -1) {
        fprintf(stderr, "Failed to setup handler for SIGUSR1\n");
    }

    //-- zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2 --
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    int child = fork();
    if(child == -1)
    {
        perror("Error while forking\n");
        exit(1);
    }
    else if (child == 0) 
    {
        // Child process
        sleep(1);
    } 
    else 
    {
        // Parent process

        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        int signo = atoi(argv[2]);
        int value = atoi(argv[2]);

        union sigval sigval;
        sigval.sival_int = value;

        sigqueue(child, signo, sigval);
    }

    return 0;
}
