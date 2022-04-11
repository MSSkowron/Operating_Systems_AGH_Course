#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <string.h>

#define BUFF_SIZE 4096

int main(int argc, char **argv) 
{
    if(argc != 4) 
    {
        perror("Wrong number of arguments!\n");
        return 1;
    }

    char *fifo_file = argv[1];
    char *output_file = argv[2];
    int chars_to_write =  atoi(argv[3]);

    FILE *fifo = fopen(fifo_file, "r");
    if(fifo == NULL) 
    {
        perror("Error while opening fifo!\n");
        return 2;
    }

    char *data = malloc(sizeof (char) * (chars_to_write + 1));
    int line;
    while(fread(&line, sizeof(int), 1, fifo)) 
    {
        FILE *file = fopen(output_file, "rw+");
        if(file == NULL) 
        {
            perror("Error while opening file!\n");
            return 3;
        }

        fread(data, sizeof (char), chars_to_write, fifo);
        flock(fileno(file), LOCK_EX);

        int cnt = 0;
        char c;
        fseek(file, 0, SEEK_SET);
        while(fread(&c, sizeof (char), 1, file)) 
        {
            if (c == '\n')
            {
                cnt++;
            }
            if (cnt == line) 
            {
                fseek(file, -1, SEEK_CUR);
                char buff[BUFF_SIZE];
                int readed = fread(buff, sizeof(char), BUFF_SIZE, file);
                fseek(file, -readed, SEEK_CUR);
                fwrite(data, sizeof(char), chars_to_write, file);
                fwrite(buff, sizeof(char), readed, file);
                break;
            }

        }
        if(cnt < line) 
        {
            // Numerowanie linii.
            for(int i = cnt + 1; i <= line; i++) 
            {
                char str[5];
                sprintf(str, "%d \n", i);
                fwrite(str, sizeof (char ), strlen(str), file);
            }
            fseek(file, -1, SEEK_CUR);
            fwrite(data, sizeof(char), chars_to_write, file);
            fwrite("\n", sizeof(char), 1, file);
        }

        flock(fileno(file), LOCK_UN);
        fclose(file);
    }

    fclose(fifo);

    return 0;
}