#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

int main(int argc, char** argv){
    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    int l = atoi(argv[0]);
    char* operand = argv[1];
    int r = atoi(argv[2]);

    if(strcmp(operand,"+") == 0){
        printf("Result is %d\n", l + r);
    }
    if(strcmp(operand,"a") == 0){
        printf("Result is %d\n", l + r);
    }
    else if(strcmp(operand, "-") == 0){
        printf("Result is %d\n", l - r);
    }
    else if(strcmp(operand, "*") == 0){
        printf("Result is %d\n", l * r);
    }
    else if(strcmp(operand, "x") == 0){
        printf("Result is %d\n", l * r);
    }
    else if(strcmp(operand,"/") == 0){
        printf("Result is %d\n", l / r);
    }

    return 0;
}