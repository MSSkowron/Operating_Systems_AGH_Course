#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define NO_COOKS 5
#define NO_DELIVERERS 5
#define TABLE_SIZE 5
#define OVEN_SIZE 5

#define PATHNAME getenv("HOME")

typedef struct sembuf sembuf;

typedef struct {
    int pizza_type[TABLE_SIZE];
    int pizza_on_table[TABLE_SIZE];
} table;

typedef struct {
    int pizza_type[OVEN_SIZE];
    int pizza_in_oven[OVEN_SIZE];
} oven;

union semun {
    int value;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

#define print_PID_TIME(pid, user) {                           \
    char buff[25];                                            \
    time_t now = time(NULL);                                  \
    strftime(buff, 25, "%Y-%m-%d %H:%M:%S", localtime(&now)); \
    printf("\n~%s\n", user);                                  \
    printf("PID: %d   TIME: %s\n", pid, buff);                \
}

#define random_range(from, to) (rand() % (to + 1 - from) + from)

#define random_sleep(from, to) usleep(random_range(from * 1000000, to * 1000000));

#endif //ZAD1_COMMON_H