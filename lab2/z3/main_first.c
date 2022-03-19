#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

int fileCounter = 0;
int dirCounter = 0;
int charCounter = 0;
int blockCounter = 0;
int fifoCounter = 0;
int slinkCounter = 0;
int socketCounter = 0;

void printFileProperties(struct stat stats,char* filename)
{

    printf("\nSciezka bezwzgledna: %s", realpath(filename,NULL));
    printf("\nLiczba dowiazan: %lu",stats.st_nlink);
    printf("\nRodzaj pliku: ");

    unsigned int mode = stats.st_mode;
    if S_ISDIR(mode)
    {
        dirCounter++;
        printf("dir");
    }
    else if S_ISREG(mode)
    {
        fileCounter++;
        printf("file");
    }
    else if S_ISBLK(mode)
    {
        blockCounter++;
        printf("block dev");
    }
    else if S_ISCHR(mode)
    {
        charCounter++;
        printf("char dev");
    }
    else if S_ISFIFO(mode)
    {
        fifoCounter++;
        printf("fifo");
    }
    else if S_ISSOCK(mode)
    {
        socketCounter++;
        printf("sock");
    }
    else if S_ISLNK(mode)
    {
        slinkCounter++;
        printf("slink");
    }

    printf("\nRozmiar: %ld bajtow",stats.st_size);
    printf("\nOstatni dostep: %s",ctime(&stats.st_atime));
    printf("Ostatnia modyfikacja: %s",ctime(&stats.st_mtime));
}

void goThroughSubDir(char* dir)
{
    struct dirent *dp;
    DIR* dfd;

    if((dfd= opendir(dir)) == NULL)
    {
        fprintf(stderr,"Can't open %s\n",dir);
        return;
    }

    char filename_qfd[257] ;
    while ((dp = readdir(dfd)) != NULL)
    {
        if(strcmp("..",dp->d_name)==0 || strcmp(".",dp->d_name)==0)
        {
            continue;
        }
        struct stat stbuf;
        sprintf( filename_qfd , "%s/%s",dir,dp->d_name);
        if( stat(filename_qfd,&stbuf ) == -1 )
        {
            printf("\nUnable to stat file: %s",filename_qfd) ;
            continue ;
        }
        if(S_ISDIR(stbuf.st_mode))
        {
            continue;
        }
        else
        {
            printf("\n------\n");
            printf("Nazwa pliku: %s",dp->d_name);
            printFileProperties(stbuf,filename_qfd);
        }
    }

    rewinddir(dfd);
    while ((dp = readdir(dfd)) != NULL)
    {
        if(strcmp("..",dp->d_name)==0 || strcmp(".",dp->d_name)==0)
        {
            continue;
        }
        struct stat stbuf;
        sprintf( filename_qfd , "%s/%s",dir,dp->d_name);
        if( stat(filename_qfd,&stbuf ) == -1 )
        {
            printf("\n------\n");
            printf("\nUnable to stat file: %s",filename_qfd) ;
            continue ;
        }
        if(S_ISDIR(stbuf.st_mode))
        {
            printf("\n------\n");
            printf("Nazwa pliku: %s",dp->d_name);
            printFileProperties(stbuf,filename_qfd);
            goThroughSubDir(filename_qfd);
        }
    }

    closedir(dfd);
}

int main(int argc,char** argv)
{
    if(argc != 2)
    {
        printf("There has to be exactly one argument which is directory!.\n");
        return 1;
    }

    struct dirent *dp;
    DIR* dfd;
    char* dir;
    dir = argv[1];
    char filename_qfd[257];

    if((dfd= opendir(dir)) == NULL)
    {
        fprintf(stderr,"Can't open %s\n",dir);
        return 0;
    }

    while ((dp = readdir(dfd)) != NULL)
    {
        if(strcmp("..",dp->d_name)==0 || strcmp(".",dp->d_name)==0)
        {
            continue;
        }

        struct stat stbuf;
        sprintf( filename_qfd , "%s/%s",dir,dp->d_name);

        if( stat(filename_qfd,&stbuf ) == -1 )
        {
            printf("\n------\n");
            printf("\nUnable to stat file: %s",filename_qfd) ;
            continue ;
        }

        if(S_ISDIR(stbuf.st_mode))
        {
            continue;
        }
        else
        {
            printf("\n------\n");
            printf("Nazwa pliku: %s",dp->d_name);
            printFileProperties(stbuf,filename_qfd);
        }
    }

    rewinddir(dfd);
    while ((dp = readdir(dfd)) != NULL)
    {
        if(strcmp("..",dp->d_name)==0 || strcmp(".",dp->d_name)==0)
        {
            continue;
        }
        struct stat stbuf;
        sprintf( filename_qfd , "%s/%s",dir,dp->d_name);
        if( stat(filename_qfd,&stbuf ) == -1 )
        {
            printf("\n------\n");
            printf("\nUnable to stat file: %s",filename_qfd) ;
            continue ;
        }
        if(S_ISDIR(stbuf.st_mode))
        {
            printf("\n------\n");
            printf("Nazwa pliku: %s",dp->d_name);
            printFileProperties(stbuf,filename_qfd);
            goThroughSubDir(filename_qfd);
        }
    }
    closedir(dfd);
    printf("\nFiles: %d",fileCounter);
    printf("\nDirectories: %d",dirCounter);
    printf("\nChar devs: %d",charCounter);
    printf("\nBlock devs: %d",blockCounter);
    printf("\nFIFO: %d",fifoCounter);
    printf("\nSlinks: %d",slinkCounter);
    printf("\nSockets: %d\n",socketCounter);
    printf("\n");
    return 0;
}