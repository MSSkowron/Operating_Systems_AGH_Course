#include "common.h"

sem_t* semaphore_TABLE;
int fd_TABLE;

// Pobieranie identyfikatora istniejącego już zbioru semaforów 
// tworzonego przez proces main'a.
void get_semaphore(){
    // Pobieramy klucz dla semafora stolu
    semaphore_TABLE = sem_open("/TABLE", 0);

    if(semaphore_TABLE == SEM_FAILED){
        perror("Error while opening table's semaphore in DELIVERER!");
        exit(EXIT_FAILURE);
    }
}

// Pobieranie pamieci wspolnej
void get_shared_memory() {
    // Utworzenie segmentu pamieci wspolnej dla stolu
    fd_TABLE = shm_open(TABLE_SHARED_MEMORY, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    
    if(fd_TABLE == -1){
        perror("Error while opening memory segment for TABLE in COOK!");
        exit(EXIT_FAILURE);
    }
}

// Zabranie pizzy ze stolu i dostarczenie do klienta
void take_pizza_off_the_table_and_deliver(){
    // Dolaczamy segmentu pamieci wspolnej dla pieca do przestrzeni adresowej procesu
    table* table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, fd_TABLE, 0);

    if(table == (void*) -1){
       perror("Error while opening memory segment for TABLE in DELIVERER!");
         exit(EXIT_FAILURE);
     }

    int pizza_type;
    int flag = 0;
    // Zabranie pizzy ze stolu
    while(flag == 0){

        // Zajmujemy dostep do stolu
        if(sem_wait(semaphore_TABLE) == -1){
        perror("Error while waiting in DELIVERER!");
        exit(EXIT_FAILURE);
        }

        for(int i = 0; i < TABLE_SIZE; i++){
            // Jesli jest pizza na tej pozycji na stole
            if(table->pizza_on_table[i] == 1){
                // Bierzemy pizze
                table->pizza_on_table[i] = 0;
                pizza_type = table->pizza_type[i];
                // Ile pizz pozostalo na stole
                int cnt = 0;
                for( int j = 0; j < TABLE_SIZE; j++) {
                    if (table->pizza_on_table[j] == 1){
                        cnt++;
                    }
                }

                // Wypisujemy informacje ze wzielismy pizze danego typu
                print_PID_TIME(getpid(), "DELIVERER");
                printf("Pobieram pizze: %d z pozycji: %d\n",pizza_type,i);
                printf("Liczba pizz na stole: %d\n", cnt);

                flag = 1;
                break;
            }
        }

        // Zwalniamy dostep do stolu
        if(sem_post(semaphore_TABLE) == -1){
        perror("Error while posting for TABLE in DELIVERER!");
        exit(EXIT_FAILURE);
        }
    }

    // Dojazd do klienta
    random_sleep(4,5);

    // Dostarczamy pizze
    print_PID_TIME(getpid(), "DELIVERER");
    printf("Dostarczam pizze: %d.\n",pizza_type);

    //Powrot od klienta
    random_sleep(4,5);

    // Odlaczamy segment pamieci
    munmap(table, sizeof(table));
}

// W przypadku otrzymania sygnalu SIGINT musze
// zamknac semafory, a takze pamiec wspolna
void handler(){
    // Zamykamy semafor
    sem_close(semaphore_TABLE);

    // Odlaczenie segmentu pamieci
    shm_unlink(TABLE_SHARED_MEMORY);

    // Konczymy dzialanie programu
    exit(0);
}

int main(int argc, char** argv){
    // Obsluga sygnalu SIGINT -> trzeba posprzatac
    signal(SIGINT, handler);

    // Pobieramy zbior semaforow
    get_semaphore();
    // Pobieramy pamiec wspolna dla STOLU
    get_shared_memory();

    // Wykonujemy prace dostawcy
    while(1){
        take_pizza_off_the_table_and_deliver();
    }

    return 0;
}