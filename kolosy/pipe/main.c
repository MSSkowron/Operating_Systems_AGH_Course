#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int pdesk[2];
	if (pipe(pdesk) == -1){
		perror("PIPE");
		return 1;
	}

	switch(fork()){
	  case -1:
		perror("FORK");
		return 2;
	  case 0:
	  	// CHILD PROCESS
		dup2(pdesk[1], STDOUT_FILENO);
		execvp("ls", argv);
		perror("EXECVP ls");
		exit(1);
	  default: {
		// PARENT PROCESS
		// Zamknij deskryptor do zapisu.
		close(pdesk[1]);
		// przekieruj deskryptor deskryptor wejścia standardowego na deskryptor końca do odczytu tego potoku.
		dup2(pdesk[0],STDIN_FILENO);
   		//wykonaj  tr a-z A-Z, w przypadku błędu  obsłuż go i wyjdź, zwracając 3.
		execlp("tr","tr","a-z","A-Z", NULL);
		perror("EXECLP tr");
		exit(3);
   /* koniec */
       	 }
	}
   return 0;
   }
