#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"

int server_queue;
int my_queue;
int my_id;

void get_message();
void stop();
void send_INIT();
void send_STOP();
void send_LIST();
void send_TO_ONE(int, char*);
void send_TO_ALL(char*);
int create_queue(const char *, int);

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

    char* path = getenv("HOME");

    key_t key = ftok(path, PROJECT_ID);

    server_queue = msgget(key, 0);

    my_queue = create_queue(path, (getpid() % ('Z' - 'a' + 1)) + 'a');

    send_INIT();

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
    if (msgrcv(my_queue, &message_received, size, -INIT - 1, 0) == -1) {
        perror("Error while receiving message\n");
        exit(1);
    }
    struct tm tm = message_received.tm;

    printf("> New message from ID: %d\n%s\n\n%d-%02d-%02d %02d:%02d:%02d\n", message_received.sender_pid, message_received.mtext,
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}

void stop() {
    msgctl(my_queue, IPC_RMID, NULL);
    send_STOP();
    exit(1);
}

void send_STOP() {
    msgbuf message_to_server;
    message_to_server.mtype = STOP;
    message_to_server.sender_pid = my_id;

    if (msgsnd(server_queue, &message_to_server, size, 0) == -1) {
        perror("Can't send STOP\n");
        exit(1);
    }
}

void send_TO_ONE(int to_id, char* message_content) {
    msgbuf message_to_server;
    message_to_server.mtype = TO_ONE;
    message_to_server.sender_pid = my_id;
    message_to_server.receiver_id = to_id;
    strncpy(message_to_server.mtext, message_content, MAX_MESSAGE_LENGTH);

    if (msgsnd(server_queue, &message_to_server, size, 0) == -1) {
        perror("Can't send TO_ONE\n");
        exit(1);
    }
}

void send_TO_ALL(char* message_content){
    msgbuf message_to_server;
    message_to_server.mtype = TO_ALL;
    message_to_server.sender_pid = my_id;
    strncpy(message_to_server.mtext, message_content, MAX_MESSAGE_LENGTH);

    if (msgsnd(server_queue, &message_to_server, size, 0) == -1) {
        perror("Can't send TO_ALL\n");
        exit(1);
    }
}

void send_INIT(){
    msgbuf message_to_server;
    message_to_server.mtype = INIT;
    message_to_server.sender_pid = getpid();
    sprintf(message_to_server.mtext, "%d", my_queue);

    if (msgsnd(server_queue, &message_to_server, size, 0) == -1) {
        perror("Can't send INIT\n");
        exit(1);
    }

    msgbuf from_server;

    printf("Waiting...\n");

    if (msgrcv(my_queue, &from_server, size, INIT, 0) == -1) {
        perror("Can't receive message\n");
        exit(1);
    }

    my_id = atoi(from_server.mtext);
    printf("Hello User! Your ID: %d\n", my_id);
    fflush(stdout);
}

void send_LIST() {
    msgbuf message_to_server;
    message_to_server.mtype = LIST;
    message_to_server.sender_pid = my_id;
    message_to_server.mtext[0] = '\0';

    if (msgsnd(server_queue, &message_to_server, size, 0) == -1) {
        perror("Can't send LIST\n");
        exit(1);
    }
}

int create_queue(const char *path, int id) {
    key_t key = ftok(path, id);
    if (key == -1) {
        perror("Error while creating key\n");
        exit(1);
    }

    int queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);

    if (queue_id == -1) {
        perror("Error while creating queue\n");
        exit(1);
    }

    return queue_id;
}
