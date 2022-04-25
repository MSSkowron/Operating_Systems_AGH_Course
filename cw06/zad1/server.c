#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "common.h"
#include <fcntl.h>

int server_queue;
int clients_PID[MAX_NO_USERS];
int clients_queue_ID[MAX_NO_USERS];

char file_name[MAX_MESSAGE_LENGTH];
FILE *file;

void send_TO_ALL(msgbuf*);
void send_TO_ONE(msgbuf*);
void menage_LIST(msgbuf*);
void menage_INIT(msgbuf*);
void message_handler(msgbuf*);
void disconnet_user(msgbuf*);
void save_log_to_file(msgbuf*);
int create_queue(const char*, int);
void destroy_queue();

int main() {
    // Registers the given function to be called at normal process termination
    if(atexit(destroy_queue) != 0){
        perror("atexit error\n");
        exit(1);
    }

    // Handle signals
    if (signal(SIGINT, destroy_queue) == SIG_ERR) {
        perror("Error while setting signal SIGINT\n");
        exit(1);
    }
    if (signal(SIGQUIT, destroy_queue) == SIG_ERR) {
        perror("Error while setting signal SIGQUIT\n");
        exit(1);
    }

    // Initialize arrays
    for (int i = 0; i < MAX_NO_USERS; i++ ) {
        clients_PID[i] = 0;
        clients_queue_ID[i] = 0;
    }

    // Raport file
    strncpy(file_name, "server_raport.txt", MAX_MESSAGE_LENGTH);

    if ((file = fopen(file_name, "w")) == NULL ) {
        perror("Error while opening raport file\n");
        exit(1);
    }

    printf("___SERVER IS RUNNING___\n");

    char* path = getenv("HOME");

    server_queue = create_queue(path, PROJECT_ID);

    // Server running
    msgbuf message;
    while(1){

        if(msgrcv(server_queue, &message, size, 0, 0) == -1){
            perror("Error while receiving message\n");
            exit(1);
        }

        message_handler(&message);
    }

    fclose(file);
}

void save_log_to_file(msgbuf *message) {
    printf("\n");
    fprintf(file,"\n");
    char buf[300];
    strncpy(buf, "", MAX_MESSAGE_LENGTH);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    switch(message->mtype) {
        case STOP:
            printf("Got STOP from: %d\n", message->sender_id);
            fprintf(file, "Got STOP from: %d\n", message->sender_id);
            break;
        case LIST:
            printf("Got LIST from: %d\n", message->sender_id);
            fprintf(file, "Got LIST from: %d\n", message->sender_id);
            break;
        case TO_ALL:
            printf("Got TO_ALL from: %d\n", message->sender_id);
            fprintf(file, "Got TO_ALL from: %d\n", message->sender_id);
            printf("Message: %s\n", message->mtext);
            fprintf(file, "Message: %s\n" , message->mtext);
            break;
        case TO_ONE:
            printf("Got TO_ONE from: %d to: %d\n", message->sender_id, message->receiver_id);
            fprintf(file, "Got TO_ONE from: %d to: %d\n", message->sender_id, message->receiver_id);
            printf("Message: %s\n", message->mtext);
            fprintf(file, "Message: %s\n" , message->mtext);
            break;
        case INIT:
            printf("New Client connected! ID: %d\n", message->sender_id);
            fprintf(file, "New Client connected! ID: %d\n", message->sender_id);
            break;
    }

    printf("%d-%02d-%02d %02d:%02d:%02d\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(file, "%d-%02d-%02d %02d:%02d:%02d\n\n\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void send_TO_ALL(msgbuf *message) {
    time_t t = time(NULL);
    message->tm = *localtime(&t);

    for (int i = 0; i < MAX_NO_USERS; i ++) {
        if (clients_PID[i] != 0 && i != message->sender_id) {
            if (msgsnd(clients_queue_ID[i], message, size, 0) == -1 ) {
                perror("Can't send message TO_ALL\n");
                exit(1);
            }
            kill(clients_PID[i], SIGUSR1);
        }
    }
}

void send_TO_ONE(msgbuf *message) {
    msgbuf message_to_send;
    message_to_send.mtype = TO_ONE;
    message_to_send.sender_id = message->sender_id;
    message_to_send.receiver_id = message->receiver_id;

    time_t t = time(NULL);
    message_to_send.tm = *localtime(&t);

    sprintf(message_to_send.mtext, "%s", message->mtext);

    if (msgsnd(clients_queue_ID[message->receiver_id], &message_to_send, size, 0) == -1 ) {
        perror("Can't send message TO_ONE\n");
        exit(1);
    }
    kill(clients_PID[message->receiver_id], SIGUSR1);
}

void menage_LIST(msgbuf *message) {
    msgbuf message_to_send;
    message_to_send.sender_id = getpid();
    message_to_send.mtype = LIST;

    time_t t = time(NULL);
    message_to_send.tm = *localtime(&t);

    strncpy(message_to_send.mtext, "", MAX_MESSAGE_LENGTH);

    strncat(message_to_send.mtext, "___USER'S LIST___\n", MAX_MESSAGE_LENGTH - strlen(message_to_send.mtext));

    strncat(message_to_send.mtext, "ID  ::  PID\n", MAX_MESSAGE_LENGTH - strlen(message_to_send.mtext));
    for (int i = 0; i < MAX_NO_USERS; i++) {
        if (clients_PID[i] != 0) {
            char tmp[20];
            sprintf(tmp, "%d   ::  %d\n", i, clients_PID[i]);
            strncat(message_to_send.mtext, tmp, MAX_MESSAGE_LENGTH - strlen(message_to_send.mtext));
        }
    }

    if (msgsnd(clients_queue_ID[message->sender_id], &message_to_send, size, 0) == -1) {
        perror("Can't send LIST\n");
        exit(1);
    }

    kill(clients_PID[message->sender_id], SIGUSR1);
}

void menage_INIT(msgbuf *msg) {
    int i = 0;
    for (; i < MAX_NO_USERS; i++) {
        if (clients_PID[i] == 0) {
            clients_PID[i] = msg->sender_id;
            clients_queue_ID[i] = atoi(msg->mtext);
            break;
        }
    }

    msgbuf message_to_send;
    message_to_send.sender_id = getpid();
    message_to_send.mtype = INIT;
    sprintf(message_to_send.mtext, "%d", i);

    msgsnd(clients_queue_ID[i], &message_to_send, size, 0);
}

void disconnet_user(msgbuf *msg) {
    int id = msg->sender_id;
    clients_PID[id] = 0;
    clients_queue_ID[id] = 0;
    printf("Disconnected User ID: %d\n", id);
}

void message_handler(msgbuf *msg) {
    switch (msg->mtype) {
        case LIST:
            menage_LIST(msg);
            break;
        case INIT:
            menage_INIT(msg);
            break;
        case TO_ONE:
            send_TO_ONE(msg);
            break;
        case TO_ALL:
            send_TO_ALL(msg);
            break;
        case STOP:
            disconnet_user(msg);
            break;
    }
    save_log_to_file(msg);
}

void destroy_queue(){
    fprintf(file, "Destroying server's queue\n");
    fclose(file);
    for (int i = 0; i < MAX_NO_USERS; i++) {
        kill(clients_PID[i], SIGINT);
    }
    msgctl(server_queue, IPC_RMID, NULL);
    exit(1);
}

int create_queue(const char *path, int id) {
    key_t key = ftok(path, id);
    if (key == -1) {
        perror("Error while creating key\n");
        exit(1);
    }

    int my_queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);

    if (my_queue_id == -1) {
        perror("Error while creating server's queue\n");
        exit(1);
    }

    return my_queue_id;
}