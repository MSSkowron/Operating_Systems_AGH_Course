#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define HUND_MILION 100000000
#include <sys/times.h>

void display_time(clock_t real, struct tms* tms_start, struct tms* tms_end);

int main(int argc, char* argv[]) 
{
    struct tms tms_start, tms_end;
    clock_t start, end;

    start = times(&tms_start);

    /* Start doin something */
    for(int i = 0; i < HUND_MILION; i++)
    {
        
    }
    /* End doin something */

    end = times(&tms_end);

    display_time(end - start, &tms_start, &tms_end);
}

void display_time(clock_t real, struct tms* tms_start, struct tms* tms_end)
{
    long clktck = sysconf(_SC_CLK_TCK);

    printf("Real   time: %f \n", real / (double)clktck);
    printf("User   time: %f \n", tms_end->tms_utime - tms_start->tms_utime / (double)clktck);
    printf("System time: %f \n", tms_end->tms_stime - tms_start->tms_stime / (double)clktck);
}