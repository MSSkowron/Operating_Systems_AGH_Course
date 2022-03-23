#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#define LENGTH 50

int main(int argc,char** argv)
{
    struct dirent * dp;
    DIR* dfd;
    char* dir;
    char* str;
    int curr_depth;
    int max_depth;
    pid_t child_pid;
    char filename_qfd[257];

    dir = argv[0];
    str = argv[1];
    curr_depth = atoi(argv[2]);
    max_depth = atoi(argv[3]);

    if(curr_depth > max_depth)
    {
        return 0;
    }

    if((dfd= opendir(dir)) == NULL)
    {
        fprintf(stderr,"Can't open %s\n",dir);
        exit(1);
    }

    while ((dp = readdir(dfd)) != NULL)
    {
        if(strcmp("..",dp->d_name)==0 || strcmp(".",dp->d_name)==0)
        {
            continue;
        }

        struct stat stbuf;
        sprintf( filename_qfd , "%s/%s",dir,dp->d_name);

        if( lstat(filename_qfd,&stbuf ) < 0 )
        {
            printf("\n------\n");
            printf("\nUnable to stat file: %s",filename_qfd) ;
            continue ;
        }

        if(S_ISREG(stbuf.st_mode))
        {
            FILE* filePointer;
            int bufferLength = 256;
            char line[bufferLength];

            filePointer = fopen(filename_qfd, "r");
            while(fgets(line, bufferLength, filePointer))
            {
                char *ptr = strstr(line, str);
                if (ptr != NULL)
                {
                    printf("\nZNALEZIONO!");
                    printf("\nSciezka: %s",filename_qfd);
                    printf("\nPID: %d\n",(int)getpid());
                    break;
                }
            }
            fclose(filePointer);
        }
    }

    char* directory = (char *) calloc(257,sizeof(char));
    char* word = (char *) calloc(LENGTH,sizeof(char));
    char* depth = (char *) calloc(LENGTH,sizeof(char));
    char* maximum_depth = (char *) calloc(LENGTH,sizeof(char));

    rewinddir(dfd);
    while ((dp = readdir(dfd)) != NULL)
    {
        if(strcmp("..",dp->d_name)==0 || strcmp(".",dp->d_name)==0)
        {
            continue;
        }
        struct stat stbuf;
        sprintf( filename_qfd , "%s/%s",dir,dp->d_name);
        if( lstat(filename_qfd,&stbuf ) < 0 )
        {
            printf("\n------\n");
            printf("\nUnable to stat file: %s",filename_qfd) ;
            continue ;
        }
        if(S_ISDIR(stbuf.st_mode))
        {
            snprintf(directory, 257, "%s", filename_qfd);
            snprintf(word, LENGTH, "%s", str);
            snprintf(depth, LENGTH, "%d", curr_depth + 1);
            snprintf(maximum_depth, LENGTH, "%d", max_depth);

            char *const parmList[] = {directory, word, depth,maximum_depth,NULL};

            child_pid = fork();
            if(child_pid == 0 ) {
                execv("./child", parmList);
            }
            wait(NULL);
        }
        memset(directory, 0, LENGTH);
        memset(word, 0, LENGTH);
        memset(depth, 0, LENGTH);
    }

    free(directory);
    free(word);
    free(depth);
    free(maximum_depth);

    closedir(dfd);
    return 0;
}