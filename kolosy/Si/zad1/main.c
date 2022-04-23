#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int sig, siginfo_t *info, void *ucontext)
{
    printf("Otrzymano wartosc: %d\n", info->si_value);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    
    action.sa_sigaction = &sighandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &action, NULL);

    int child = fork();
    if(child == -1){
        perror("Fork error!\n");
        exit(1);
    }
    else if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
        sigprocmask(SIG_BLOCK, &mask, NULL);
        sigaction(SIGUSR1, &action, NULL);
        pause();
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        sleep(1);

        union sigval sv;

        sv.sival_int = atoi(argv[1]);
        int signal = atoi(argv[2]);

        sigqueue(child, signal, sv);
    }
    return 0;
}
