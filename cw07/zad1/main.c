#include "common.h"

int semaphores_SET_ID;

int oven_ID;
int table_ID;

pid_t cooks_PIDS[NO_COOKS];
pid_t deliverers_PIDS[NO_DELIVERERS];

// Ustawienie semaforow
void set_semaphores() {
    // Tworzymy klucz dla zbioru semaforow
    key_t semaphore_key = ftok(PATHNAME,'S');
    
    if(semaphore_key == -1){
        perror("Error while creating key for SEMAPHORE!");
        exit(EXIT_FAILURE);
    }

    // Tworzymy zbior semaforow
    // 0 - PIEC
    // 1 - STOL
    // Skladac sie bedzie z dwoch semaforow - jeden dla pieca, jeden dla stolu
    if ((semaphores_SET_ID = semget(semaphore_key, 2, IPC_CREAT | 0666))== -1){
        perror("Error while creating semaphore's SET!");
        exit(EXIT_FAILURE);
    }

    // Unie bitowe prezentujace dodatkowe informacje dla semaforow
    union semun table_arg;
    union semun oven_arg;

    // Poczatkowe wartosci semaforow
    table_arg.value = 1;
    oven_arg.value = 1;

    // Ustawiamy wartosc semafora dla pieca
    if (semctl(semaphores_SET_ID, 0, SETVAL, oven_arg) == -1){
        perror("Error while setting oven's semaphore value!");
        exit(EXIT_FAILURE);
    }

    // Ustawiamy wartosc semafora dla stolu
        if (semctl(semaphores_SET_ID, 1, SETVAL, table_arg) == -1){
        perror("Error while setting table's semaphore value!");
        exit(EXIT_FAILURE);
    }
}


// Tworzenie pamieci wspolnej dla procesow
void create_shared_memory() {

    // Blok pamieci dla STOLU
    key_t table_key = ftok(PATHNAME, 'T');
    if(table_key == -1){
        perror("Error while creating key for TABLE!");
        exit(EXIT_FAILURE);
    }

    // Tworzymy segment pamieci dla STOLU
    if((table_ID = shmget(table_key, sizeof(table),IPC_CREAT | 0666)) == -1){
        perror("Error while creating shared memory for TABLE!");
        exit(EXIT_FAILURE);
    }

    // Blok pamieci dla PIECA
    key_t oven_key = ftok(PATHNAME, 'O');
    if(oven_key == -1){
        perror("Error while creating key for OVEN!");
        exit(EXIT_FAILURE);
    }

    // Tworzymy segment pamieci dla PIECA
    if((oven_ID = shmget(oven_key, sizeof(oven),IPC_CREAT | 0666)) == -1){
        perror("Error while creating shared memory for OVEN!");
        exit(EXIT_FAILURE);
    }
}

// W przypadku otrzymania sygnalu SIGINT musze zabic wszystkie procesy potomne
// Oraz usunac zbior semaforow z systemu, a takze pamiec wspolna
void handler(){
    // Zabijamy procesy potomne
    for(int i = 0; i < NO_COOKS; i++){
        kill(cooks_PIDS[i], SIGINT);
    }
    for(int i = 0; i < NO_DELIVERERS; i++){
        kill(deliverers_PIDS[i], SIGINT);
    }

    // Usuwamy zbior semaforow z systemu
    semctl(semaphores_SET_ID, 0, IPC_RMID, NULL);
    // Usuwamy segment pamięci wspólnej dla PIECA z systemu 
    shmctl(oven_ID, IPC_RMID, NULL);
    // Usuwamy segment pamięci wspólnej dla PIECA z systemu 
    shmctl(table_ID, IPC_RMID, NULL);

    // Konczymy dzialanie programu
    exit(0);
}

int main(int argc, char** argv) {
    // Obsluga sygnalu SIGINT -> trzeba posprzatac
    signal(SIGINT, handler);

    // Ustawiamy zbior semaforow
    set_semaphores();
    // Tworzymy pamieci wspolnej dla STOLU i PIECA
    create_shared_memory();

    // Uruchamiamy kucharzy
    for(int i = 0; i < NO_COOKS; i++){
        pid_t child = fork();

        if(child == -1){
            perror("Error while creating child process for COOKS!");
            exit(EXIT_FAILURE);
        }
        else if (child == 0){
            // CHILD PROCESS
            execl("./cook", "cook", NULL);
        }
        else {
            // PARENT PROCESS
            // Zapisuje kucharza do tablicy kucharzy
            cooks_PIDS[i] = child;
        }
        sleep(1);
    }

    sleep(1);

    // Uruchamiamy dostawcow
    for(int i = 1; i < NO_DELIVERERS; i++){
        pid_t child = fork();
        if(child == -1){
            perror("Error while creating child process for DELIVERERS!");
            exit(EXIT_FAILURE);
        }
        else if (child == 0){
            // CHILD PROCESS
            execl("./deliverer", "deliverer", NULL);
        }
        else {
            // PARENT PROCESS
            // Zapisuje dostawce do tablicy dostawcow
            deliverers_PIDS[i] = child;
        }
        sleep(1);
    }

    // Czekamy na zakonczenia procesow potomnych
    for(int i = 0; i < NO_COOKS; i++){
        wait(NULL);
    }
    for(int i = 0; i < NO_DELIVERERS; i++){
        wait(NULL);
    }

    while(1) {}

    return 0;
}
