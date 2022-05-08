#include "common.h"

pid_t cooks_PIDS[NO_COOKS];
pid_t deliverers_PIDS[NO_DELIVERERS];

int oven_MEMORY;
int table_MEMORY;

// Ustawienie semaforow
void set_semaphores() {
    // Utworzenie semafora dla stolu
    sem_t *semaphore = sem_open("/TABLE", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, TABLE_SIZE);
    
    if(semaphore == SEM_FAILED){
        perror("Error while creating table's semaphore!");
        exit(EXIT_FAILURE);
    }

    // Zamkniecie semafora
    sem_close(semaphore);

    // Utworzenie semafora dla pieca
    semaphore = sem_open("/OVEN", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, OVEN_SIZE);
    
    if(semaphore == SEM_FAILED){
        perror("Error while creating oven's semaphore!");
        exit(EXIT_FAILURE);
    }

    // Zamkniecie semafora
    sem_close(semaphore);
}


// Tworzenie pamieci wspolnej dla procesow
void create_shared_memory() {
    // Utworzenie segmentu pamieci wspolnej dla pieca
    int fd1 = shm_open(OVEN_SHARED_MEMORY, O_RDWR | O_CREAT, 0666);
    if(fd1 == -1){
        perror("Error while creating memory segment for OVEN!");
        exit(EXIT_FAILURE);
    }
    // Okreslnie rozmiaru segmentu pamieci dla pieca
    if(ftruncate(fd1, sizeof(oven)) == -1){
        perror("Error while determining segment memory size for OVEN!");
        exit(EXIT_FAILURE);
    }

    // Utworzenie segmentu pamieci wspolnej dla stolu
    int fd2 = shm_open(TABLE_SHARED_MEMORY, O_RDWR | O_CREAT, 0666);
    if(fd2 == -1){
        perror("Error while creating memory segment for TABLE!");
        exit(EXIT_FAILURE);
    }
    // Okreslnie rozmiaru segmentu pamieci dla pieca
    if(ftruncate(fd2, sizeof(table)) == -1){
        perror("Error while determining segment memory size for TABLE!");
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

    // Usuniecie semaforow
    sem_unlink("/OVEN");
    sem_unlink("/TABLE");
    
    // Usuniecie segmentow pamieci
    shm_unlink(TABLE_SHARED_MEMORY);
    shm_unlink(OVEN_SHARED_MEMORY);

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
