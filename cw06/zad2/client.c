#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>           
#include <sys/stat.h>        
#include <mqueue.h>

#include "common.h"

mqd_t server_queue;
mqd_t my_queue;
int my_id;
char path[250];

void get_message();
void stop();
void send_INIT(const char*);
void send_STOP();
void send_LIST();
void send_TO_ONE(int, char*);
void send_TO_ALL(char*);

int main() {
    if (atexit(stop) != 0) {
        perror("atexit error\n");
        exit(1);
    }

    if (signal(SIGINT, stop) == SIG_ERR || signal(SIGQUIT, stop) == SIG_ERR) {
        printf("Error while setting signal INT/QUIT handler\n");
        exit(1);
    }
    if (signal(SIGUSR1, get_message) == SIG_ERR) {
        perror("Error while setting signal SIGUSR1 handler\n");
        exit(1);
    }

    sprintf((char*) path, "/%d", getpid());


    server_queue = open_queue(SERVER);
    my_queue = create_queue(path);

    send_INIT(path);

    struct sigevent notify_handler;
    notify_handler.sigev_notify = SIGEV_SIGNAL;
    notify_handler.sigev_signo = SIGUSR1;

    mq_notify(my_queue, &notify_handler);

    int receiver_id = -1;
    char input[MAX_MESSAGE_LENGTH + 1];
    const char delimiter[] = " ,\n";
    char * pch;
    char message_content[MAX_MESSAGE_LENGTH];

    while (1) {
        message_content[0] = '\0';
        fgets(input, MAX_MESSAGE_LENGTH, stdin);
        input[MAX_MESSAGE_LENGTH] = 0;
        pch = strtok(input, delimiter);

        if (strcmp(pch, "LIST") == 0) {
            send_LIST();
        }
        else if (strcmp(pch, "STOP") == 0) {
            stop();
        }
        else if (strcmp(pch, "2ONE") == 0) {
            receiver_id = -1;

            pch = strtok(NULL, " ");
            receiver_id = atoi(pch);

            pch = strtok(NULL, "\n");
            strncat(message_content, pch, MAX_MESSAGE_LENGTH);

            send_TO_ONE(receiver_id, message_content);
        }
        else if(strcmp(pch, "2ALL") == 0) {
            pch = strtok(NULL, "\n");
            strncat(message_content, pch, MAX_MESSAGE_LENGTH);
            send_TO_ALL(message_content);
        }

    }
}

void get_message() {
    msgbuf message_received;
    uint mtype;

    if (mq_receive(my_queue, (char *) &message_received, size, &mtype) == -1) {
        perror("Error while receiving message\n");
        exit(1);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("> New message from ID: %d\n%s\n\n%d-%02d-%02d %02d:%02d:%02d\n", message_received.sender_id, message_received.mtext,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}

void stop() {
    send_STOP();
    mq_close(server_queue);
    mq_unlink(path);
    exit(1);
}

void send_STOP() {
    msgbuf message_to_server;
    message_to_server.sender_id = my_id;
    message_to_server.mtext[0] = '\0';
    mq_send(server_queue, (char *) &message_to_server, size, STOP);
}

void send_TO_ONE(int to_id, char* message_content) {
    msgbuf message_to_server;
    message_to_server.sender_id = my_id;
    message_to_server.receiver_id = to_id;
    strncpy(message_to_server.mtext, message_content, MAX_MESSAGE_LENGTH);

    if (mq_send(server_queue, (char *) &message_to_server, size, TO_ONE) == -1) {
        perror("Can't send TO_ONE\n");
        exit(1);
    }
}

void send_TO_ALL(char* message_content){
    msgbuf message_to_server;
    message_to_server.sender_id = my_id;
    strncpy(message_to_server.mtext, message_content, MAX_MESSAGE_LENGTH);

    if (mq_send(server_queue, (char *) &message_to_server, size, TO_ALL) == -1) {
        perror("Can't send TO_ALL\n");
        exit(1);
    }
}

void send_INIT(const char* path){
    msgbuf message_to_server;
    message_to_server.sender_id = getpid();
    sprintf(message_to_server.mtext, "%s", path);


    if (mq_send(server_queue, (char *) &message_to_server, size, INIT) == -1) {
        perror("Can't send INIT\n");
        exit(1);
    }

    msgbuf message_from_server;

    printf("Waiting...\n");

    if (mq_receive(my_queue, &message_from_server, size, NULL) == -1) {
        perror("Can't receive message from server\n");
        exit(1);
    }

    my_id = atoi(message_from_server.mtext);
    printf("Hello User! Your ID: %d\n", my_id);
    fflush(stdout);
}

void send_LIST() {
    msgbuf message_to_server;
    message_to_server.sender_id = my_id;
    message_to_server.mtext[0] = '\0';

    if (mq_send(server_queue, (char *) &message_to_server, size, LIST) == -1) {
        perror("Can't send LIST\n");
        exit(1);
    }
}
