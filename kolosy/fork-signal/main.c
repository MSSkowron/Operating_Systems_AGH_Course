#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int main(int argc, char *argv[])
{
  if (argc!=2) {
     printf("Prawidłowe wywołanie %s liczba\n",argv[0]); 
     exit(EXIT_FAILURE);
  }
  pid_t child;
  int status;
  if((child = fork()) < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if(child == 0) {
    // CHILD 
    sleep(2);
    exit(EXIT_SUCCESS);
  }
  else {
    // PARENT
    // Proces macierzysty usypia na liczbę sekund będącą argumentem programu
    sleep(atoi(argv[1]));
    // Proces macierzysty pobiera status  zakończenia potomka child, nie zawieszając swojej pracy. 
    int status;
    waitpid(child,&status, WNOHANG);
    // Jeśli proces się nie zakończył, wysyła do dziecka sygnał zakończenia.
    if(status == 0){
      int result = kill(child, SIGKILL);
      // Jeśli wysłanie sygnału się nie powiodło, proces zwróci błąd.
      if(result == -1){
        // ERROR
        return 1;
        
      } // Jeśli się powiodło, wypisuje komunikat sukcesu zakończenia procesu potomka z numerem jego PID i sposobem zakończenia
      else {
        printf("Proces %d zakonczony sygnalem!\n", child);
      }
    } else {
      printf("Proces %d zakonczony przez exit\n",child);
    }

 
 
 
 
 
 
 
 
 
 } //else
  return 0;
}
