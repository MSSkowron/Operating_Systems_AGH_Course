#include <stdio.h>
#include <unistd.h>

int main(int argc,char** argv)
{
    printf("\nMoje PID : %d\n", (int)getpid());
    return 0;
}