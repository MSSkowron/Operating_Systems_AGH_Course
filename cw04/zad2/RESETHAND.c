//
// Created by mateuszskowron on 30.03.2022.
//
#include <stdio.h>
#include <signal.h>

void signal_handler(int sig) {
    printf("Signal received.\n");
}

int main() {
    struct sigaction act;
    act.sa_handler = signal_handler;
    act.sa_flags = SA_RESETHAND;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);

    /*
     * Pierwsze wysłanie sygnału jesteśmy w stanie obsłużyć naszym handlerem.
     * Obsługując nasz sygnał kernel resetuje nasz handler na domyślny,
     * czyli nasz proces jest zabijany, gdy przychodzi po raz drugi.
     */
    raise(SIGUSR1);
    raise(SIGUSR1);
}

