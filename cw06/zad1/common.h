#ifndef CW06_COMMON_H
#define CW06_COMMON_H

#include "signal.h"
#include <time.h>

#define PROJECT_ID 'P'
#define MAX_MESSAGE_LENGTH 250
#define MAX_NO_USERS 20

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5

typedef struct {
    long mtype;
    char mtext[MAX_MESSAGE_LENGTH];
    int sender_pid;
    int receiver_id;
    struct tm tm;
} msgbuf;

int size = sizeof(msgbuf) - sizeof(long);

#endif //CW06_COMMON_H
