#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

int main(int argc, char** argv)
{
   if(argc != 4){
      printf("Not a suitable number of program parameters\n");
      return 1;
   }
   //utworz proces potomny w ktorym wykonasz program ./calc z parametrami argv[1], argv[2] oraz argv[3]
   //odpowiednio jako pierwszy operand, operacja (+-x/) i drugi operand 
   //uzyj tablicowego sposobu przekazywania parametrow do programu 
   pid_t child_pid = fork();
   if(child_pid == -1){
      printf("Error while forking\n");
      return 1;
   }
   else if(child_pid == 0){
      char* const args[] = {argv[1], argv[2], argv[3], NULL};
      execv("./calc", args);
      // Child process

   } else {
      // Parent process
      wait(NULL);
   }
   return 0;
}
