#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("\nMoje PID : %d\n", (int)getpid());
    return 0;
}