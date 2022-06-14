#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#ifndef CW10_ZAD1_H
#define CW10_ZAD1_H

#define MAX_MSG_LENGTH 100
#define MAX_NO_CLIENTS 10

typedef struct{
    char *name;
    int is_in_game;
    int opponent;
    int is_online;
    int fd;
} client;

typedef enum{
    GAME_NOT_STARTED,
    LOOKING_FOR_OPPONENT,
    OPPONENTS_TURN,
    OPPONENTS_TURN_FINISHED,
    MY_TURN,
    GAME_OVER
} game_status;

typedef enum{
    EMPTY,
    O,
    X
} board_field;

typedef struct{
    board_field fields[9];
} board;

#endif //CW10_ZAD1_H