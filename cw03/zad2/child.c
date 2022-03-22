#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

float func(float x)
{
    return  4.0/((x*x) + 1);
}

int main(int argc,char** argv)
{
    double middle = atof(argv[0]);
    int n = atof(argv[1]);
    float width = atof(argv[2]);
    float result = 0.0;

    for(int i = 0; i < n; i++)
    {
        result += width * func(middle);
        middle += width;
    }

    char* filename = (char *) calloc(50,sizeof(char));
    strcat(filename,"w");
    strcat(filename,argv[3]);
    strcat(filename,".txt");

    FILE * file;
    file = fopen(filename,"w");

    fprintf(file, "%f", result);

    free(filename);
    fclose(file);
    return 0;
}