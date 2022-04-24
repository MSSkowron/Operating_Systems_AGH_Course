#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main(int argc, char* argv[]) {

 if(argc !=2){
   printf("Not a suitable number of program parameters\n");
   return(1);
 } 

  //utworz potok nazwany pod sciezka reprezentowana przez PIPE
  mkfifo(PIPE,0777);
  //zakladajac ze parametrem wywolania programu jest liczba calkowita
  //zapisz te wartosc jako int do potoku i posprzataj
  int value = atoi(argv[1]);

  FILE *file = fopen(PIPE, "w");
  if(file == NULL){
    printf("Can't open PIPE\n");
    return 1;
  }

  fwrite(&value, sizeof(int), 1, file);

  fclose(file);

  return 0;
}
