#include "common.h"

int semaphores_SET_ID;
int table_ID;

// Pobieranie identyfikatora istniejącego już zbioru semaforów 
// tworzonego przez proces main'a.
void get_semaphores_SET_ID(){
    // Pobieramy klucz dla zbioru semaforow
    key_t semaphore_key = ftok(PATHNAME,'S');

    // Pobieramy identyfikator zbioru semaforow
    if((semaphores_SET_ID = semget(semaphore_key, 0, 0)) == -1){
        perror("Error while getting semaphores SET ID in COOK!");
        exit(EXIT_FAILURE);
    }
}

// Pobieranie pamieci wspolnej
void get_shared_memory() {
    // Blok pamieci dla STOLU
    key_t table_key = ftok(PATHNAME, 'T');
    if(table_key == -1){
        perror("Error while getting key for TABLE in COOK!");
        exit(EXIT_FAILURE);
    }

    // Pobieramy segment pamieci dla STOLU
    if((table_ID = shmget(table_key, 0, 0)) == -1){
        perror("Error while getting shared memory for TABLE in COOK!");
        exit(EXIT_FAILURE);
    }
}

// Zabranie pizzy ze stolu i dostarczenie do klienta
void take_pizza_off_the_table_and_deliver(){
    // Dolaczenie segmentu pamieci wspolnej do przestrzeni adresowej procesu
    table *table = shmat(table_ID, NULL, 0);

    sembuf *sem_table_1 = calloc(1, sizeof(sembuf));
    sembuf *sem_table_2 = calloc(1, sizeof(sembuf));

    int pizza_type;

    int flag = 0;
    while(flag == 0){

        // Zajmujemy dostep do stolu
        sem_table_1->sem_op = -1;
        sem_table_1->sem_num = 1;
        sem_table_1->sem_flg = SEM_UNDO;
        semop(semaphores_SET_ID, sem_table_1, 1);

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
        sem_table_2->sem_op = 1;
        sem_table_2->sem_num = 1;
        sem_table_2->sem_flg = SEM_UNDO;
        semop(semaphores_SET_ID, sem_table_2, 1);
    }

    // Dojazd do klienta
    random_sleep(4,5);

    // Dostarczenie pizzy
    print_PID_TIME(getpid(), "DELIVERER");
    printf("Dostarczam pizze: %d.\n",pizza_type);

    //Powrot od klienta
    random_sleep(4,5);

    // Odlaczenie segmentu pamieci
    shmdt(table);
    // Zwolnienie zaalokowanej pamieci
    free(sem_table_1);
    free(sem_table_2);
}

// W przypadku otrzymania sygnalu SIGINT musze
// usunac zbior semaforow z systemu, a takze pamiec wspolna
void handler(){
    // Usuwamy zbior semaforow z systemu
    semctl(semaphores_SET_ID, 0, IPC_RMID, NULL);
    // Usuwamy segment pamięci wspólnej dla PIECA z systemu 
    shmctl(table_ID, IPC_RMID, NULL);

    // Konczymy dzialanie programu
    exit(0);
}

int main(int argc, char** argv){
    // Obsluga sygnalu SIGINT -> trzeba posprzatac
    signal(SIGINT, handler);

    // Pobieramy zbior semaforow
    get_semaphores_SET_ID();
    // Pobieramy pamiec wspolna dla STOLU
    get_shared_memory();

    // Wykonujemy prace dostawcy
    while(1){
        take_pizza_off_the_table_and_deliver();
    }

    return 0;
}