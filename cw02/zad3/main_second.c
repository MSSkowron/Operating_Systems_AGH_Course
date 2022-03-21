#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define __USE_XOPEN_EXTENDED 1
#include <ftw.h>

char* dirName;
int fileCounter = 0;
int dirCounter = 0;
int charCounter = 0;
int blockCounter = 0;
int fifoCounter = 0;
int slinkCounter = 0;
int socketCounter = 0;

void printFileProperties(const struct stat *stats,const char* filename)
{
    printf("\nNazwa pliku: %s",filename);
    printf("\nSciezka bezwzgledna: %s", realpath(filename,NULL));
    printf("\nLiczba dowiazan: %lu",stats->st_nlink);
    printf("\nRodzaj pliku: ");

    unsigned int mode = stats->st_mode;
    if S_ISDIR(mode)
    {
        dirCounter++;
        printf("dir");
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
    else if S_ISREG(mode)
    {
        fileCounter++;
        printf("file");
    }

    printf("\nRozmiar: %ld bajtow",stats->st_size);
    printf("\nOstatni dostep: %s",ctime(&stats->st_atime));
    printf("Ostatnia modyfikacja: %s",ctime(&stats->st_mtime));
}

int nftwfunc(const char* filename,const struct stat *statptr,int fileflags,struct FTW *pfwt)
{
    char* name = malloc(sizeof(char)*256);
    int len = strlen(filename);
    int i = len - 1;
    int j = 0;
    while (i>=0 && filename[i]!='/')
    {
        i--;
    }
    i++;
    while (i < len)
    {
        name[j] = filename[i];
        i++;
        j++;
    }
    name[j]='\0';
    if(strcmp(dirName,name)!=0)
    {
        printFileProperties(statptr,name);
    }
    free(name);
    return 0;
}

int main(int argc,char** argv)
{
    if(argc != 2)
    {
        printf("There has to be exactly one argument which is directory!.\n");
        return 1;
    }

    char* dir;
    dir = argv[1];
    dirName = argv[1];
    int fd_limits = 5;
    int flags = FTW_CHDIR | FTW_MOUNT| FTW_PHYS ;
    int ret;

    ret = nftw(dir, nftwfunc,fd_limits,flags);
    if(ret == -1)
    {
        printf("Something went wrong!\n");
        return 1;
    }

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