#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>

int main(int argc, char **argv) 
{
    if(argc != 5) 
    {
        perror("Wrong number of arguments!\n");
        return 1;
    }

    char* fifo_file = argv[1];
    int  producer_number = atoi(argv[2]);
    char* input_file = argv[3];
    int characters_to_read = atoi(argv[4]);

    // Otwarcie pliku z ktorego bedziemy czytac.
    FILE *file = fopen(input_file, "r");
    if(file == NULL) 
    {
        perror("Error while opening file!\n");
        return 2;
    }

    // Otwarcie pliku fifo do ktorego bedziemy pisac.
    FILE *fifo = fopen(fifo_file, "w");
    if(fifo == NULL) 
    {
        perror("Error while opening fifo!\n");
        return 3;
    }
    
    sleep(1);
    char *data = malloc(sizeof (char) * characters_to_read);
    while(fread(data, sizeof (char), characters_to_read, file) == characters_to_read) 
    {
        flock(fileno(fifo), LOCK_EX);
        sleep(1);
        fwrite(&producer_number,sizeof(int), 1, fifo);
        fwrite(data, sizeof (char), characters_to_read, fifo);
        flock(fileno(fifo), LOCK_UN);
    }

    fclose(file);
    fclose(fifo);

    return 0;
}