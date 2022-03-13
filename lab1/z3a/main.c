#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>
#include "mylib.h"

FILE * raportFile;

//https://stackoverflow.com/questions/5309471/getting-file-extension-in-c
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

double get_time_difference(clock_t start, clock_t end)
{
    return (double) (end-start) / sysconf(_SC_CLK_TCK);
}

void print_time_result(clock_t start, clock_t end, struct tms* t_start, struct tms* t_end)
{
    printf("\n");
    printf("REAL_TIME: %fl\n", get_time_difference(start,end));
    printf("USER_TIME: %fl\n", get_time_difference(t_start->tms_utime, t_end->tms_utime));
    printf("SYSTEM_TIME: %fl\n", get_time_difference(t_start->tms_stime, t_end->tms_stime));

    fprintf(raportFile, "REAL_TIME: %fl\n", get_time_difference(start, end));
    fprintf(raportFile, "USER_TIME: %fl\n", get_time_difference(t_start->tms_utime, t_end->tms_utime));
    fprintf(raportFile, "SYSTEM_TIME: %fl\n", get_time_difference(t_start->tms_stime, t_end->tms_stime));
    printf("\n");
}

int main(int argc,char **argv){
    if(argc < 2)
    {
        printf("Too few arguments");
        return -1;
    }

    raportFile = fopen("./results3a.txt","a");

    struct tms* tms[argc];
    clock_t time[argc];
    for(int i = 0; i <argc; i++)
    {
        tms[i] = calloc(1,sizeof(struct tms *));
        time[i] = 0;
    }

    int k = 0;

    printf("\n");

    BlocksArray* blocksArray = NULL;

    int i = 2;
    while (i < argc)
    {
        time[k] = times(tms[k]);
        k += 1;

        if(strcmp("create_table",argv[i]) ==0 )
        {
            if(i + 1 >= argc || is_number(argv[i+1]) == -1)
            {
                printf("Invalid/Lack of parameter for create_table command\n");
                fprintf(raportFile,"Invalid/Lack of parameter for create_table command\n");
                break;
            }
            int max_size = atoi(argv[i+1]);
            printf("Created table of size: %d\n",max_size);
            fprintf(raportFile,"Created table of size %d:\n",max_size);
            blocksArray = createArrayOfBlocks(max_size);
            i += 2;
        }
        else if(strcmp("wc_files",argv[i]) == 0)
        {
            if(i + 1 >= argc || strcmp("txt", get_filename_ext(argv[i + 1])) != 0) {
                printf("Invalid/Lack of parameter for wc_files command\n");
                break;
            }

            int txt_files_counter = 0;
            int j = i + 1;
            while (j < argc)
            {
                if(strcmp("txt", get_filename_ext(argv[j])) == 0)
                {
                    txt_files_counter++;
                }
                else
                {
                    break;
                }
                j++;
            }

            j = i + 1;
            while(j < i + 1 + txt_files_counter)
            {
                char* file = argv[j];
                printf("Counting... file: %s\n",file);
                fprintf(raportFile,"Counting... file: %s\n",file);
                create_temp_file();
                count(blocksArray,file,"temp.txt");
                delete_temp_file();
                j++;
            }
            i = i + 1 + txt_files_counter;
        }

        else if(strcmp("remove_block",argv[i]) == 0)
        {
            if(i + 1 >= argc || is_number(argv[i+1]) == -1)
            {
                printf("Invalid/Lack of parameter for remove_block command.\n");
                fprintf(raportFile,"Invalid/Lack of parameter for remove_block command.\n");
                break;
            }

            int index = atoi(argv[i+1]);
            printf("Removed block at index: %d\n",index);
            fprintf(raportFile,"Removed block at index: %d\n",index);
            int result = removeBlock(blocksArray,index);
            if(result == -1)
            {
                printf("Nothing to remove/Index out of range.\n");
                fprintf(raportFile,"Nothing to remove/Index out of range.\n");
                break;
            }
            i+=2;
        }
        else if(strcmp("print_table",argv[i]) == 0)
        {
            printArrayOfBlocks(blocksArray);
            fprintf(raportFile,"Printed table.\n");
            i += 1;
        }
        else
        {
            printf("Invalid argument.\n");
            fprintf(raportFile,"Invalid argument.\n");
            break;
        }

        time[k] = times(tms[k]);
        print_time_result(time[k-1],time[k],tms[k-1],tms[k]);
        k +=1 ;
    }

    removeArrayOfBlocks(blocksArray);
    fprintf(raportFile,"--------------!!!--------------\n");
    fprintf(raportFile,"--------------!!!--------------\n");
    fclose(raportFile);
    return 0;
}