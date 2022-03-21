#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "functions.h"

char * getInput(){
    const size_t bufSize = 50;
    char tmp[bufSize];
    char * input = malloc(sizeof(char)*bufSize);
    size_t offset = 0;
    while(fgets(tmp,sizeof(tmp),stdin) != NULL)
    {
        strncpy(input + offset,tmp,bufSize);
        offset += bufSize;
        input = realloc(input,bufSize+offset);
        for(size_t i = 0; i < bufSize; i++)
        {
            if(tmp[i] == '\n')
            {
                input[i+offset-bufSize] = 0x0;
                return input;
            }
        }
    }
    return input;
}

char ** getFiles(int argc,char ** argv){
    char ** files = malloc(sizeof(char**)*2);
    if(argc < 3)
    {
        if(argc < 2)
        {
            puts("Name of the first file: ");
            files[0] = getInput();
        }
        else
        {
            files[0] = argv[1];
        }
        puts("Name of the second file: ");
        files[1] = getInput();
    }
    else
    {
        files[0] = argv[1];
        files[1] = argv[2];
    }

    return files;
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}
