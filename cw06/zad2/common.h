#ifndef CW06_COMMON_H
#define CW06_COMMON_H

#include "signal.h"
#include <time.h>
#include <fcntl.h>          
#include <sys/stat.h>        
#include <mqueue.h>

#define PROJECT_ID 'P'
#define SERVER "/server_queue"
#define MAX_MESSAGE_LENGTH 250
#define MAX_NO_USERS 20

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5

typedef struct {
    int sender_id;
    int receiver_id;
    char mtext[MAX_MESSAGE_LENGTH];
} msgbuf;

int size = sizeof(msgbuf);

mqd_t create_queue(const char* path){
    struct mq_attr attr;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = size;

    mqd_t queue_id = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0660, &attr);

    if(queue_id == -1){
        perror("Error while creating queue\n");
        exit(1);
    }

    return queue_id;
}

mqd_t open_queue(const char* path){
    mqd_t queue_id = mq_open(path, O_WRONLY);

    if(queue_id == -1){
        perror("Error while opening queue\n");
        exit(1);
    }

    return queue_id;
}

#endif //CW06_COMMON_H
