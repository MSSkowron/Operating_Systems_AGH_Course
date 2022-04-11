#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 256
#define EMAIL_ADDRESS_MAX_SIZE 100
#define TITLE_MAX_SIZE 100
#define CONTENT_MAX_SIZE 1000

int main(int argc, char** argv)
{
    if(argc == 4)
    {
        if(strlen(argv[1]) > EMAIL_ADDRESS_MAX_SIZE)
        {
            perror("Too long email address. Max size is 100!\n");
            exit(1);
        }
        if(strlen(argv[2]) > TITLE_MAX_SIZE)
        {
            perror("Too long title. Max size is 100!\n");
            exit(1);
        }
        if(strlen(argv[3]) > CONTENT_MAX_SIZE)
        {
            perror("Too long content. Max size is 1000!\n");
            exit(1);
        }

        char* email_address = calloc(EMAIL_ADDRESS_MAX_SIZE, sizeof(char));
        char* title = calloc(TITLE_MAX_SIZE, sizeof(char));
        char* content = calloc(CONTENT_MAX_SIZE, sizeof(char));

        strcpy(email_address, argv[1]);
        strcpy(title, argv[2]);
        strcpy(content, argv[3]);

        char* command = calloc(EMAIL_ADDRESS_MAX_SIZE + TITLE_MAX_SIZE + CONTENT_MAX_SIZE + 20, sizeof(char));
        sprintf(command, "echo %s | mail %s -s %s",content, email_address, title);

        FILE* mail_fp = popen(command, "w");
        if(mail_fp == NULL)
        {
            perror("Pipe initialization error!\n");
            exit(1);
        }
        pclose(mail_fp);

        printf("\nSent to: %s\n",email_address);
        printf("Subject: %s\n",title);
        printf("Text:    %s\n\n",content);

        free(command);
        free(email_address);
        free(title);
        free(content);
    }
    else if(argc == 2)
    {
        if(strcmp(argv[1], "nadawca") == 0)
        {
            FILE* mails_fp = popen("mail -H | sort -k 3", "r");
            if(mails_fp == NULL)
            {
            perror("Pipe initialization error!\n");
            exit(1);
            }

            char* line = calloc(LINE_SIZE, sizeof(char));
            size_t buff_size = LINE_SIZE;

            printf("Mails sorted by nadawca\n");
            
            while(getline(&line, &buff_size, mails_fp) != EOF)
            {
                printf("%s", line);
            }
            pclose(mails_fp);

        }
        else if(strcmp(argv[1], "data") == 0)
        {
            FILE* mails_fp = popen("mail -H | sort -k 7 | sort -k 6 | sort -k 5", "r");
            if(mails_fp == NULL)
            {
            perror("Pipe initialization error!\n");
            exit(1);
            }
            char* line = calloc(LINE_SIZE, sizeof(char));
            size_t buff_size = LINE_SIZE;

            printf("\nMails sorted by data\n");

            while(getline(&line, &buff_size, mails_fp) != EOF)
            {
                printf("%s", line);
            }
            pclose(mails_fp);
        }
        else{
            perror("./main [nadawca || data]\n");
            exit(1);
        }
    }
    else{
        perror("Invalid number of arguments!\n");
        exit(1);
    }
    return 0;
}