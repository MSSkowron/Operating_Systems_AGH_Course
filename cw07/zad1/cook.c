#include "common.h"

int semaphores_SET_ID;

int oven_ID;
int table_ID;

int pizza_TYPE;
int place_in_oven;

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

// Pobieramy pamieci wspolnych
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

    // Blok pamieci dla PIECA
    key_t oven_key = ftok(PATHNAME, 'O');
    if(oven_key == -1){
        perror("Error while getting key for OVEN in COOK");
        exit(EXIT_FAILURE);
    }

    // Pobieramy segment pamieci dla PIECA
    if((oven_ID = shmget(oven_key, 0, 0)) == -1){
        perror("Error while getting shared memory for OVEN in COOK!");
        exit(EXIT_FAILURE);
    }
}

// Umieszczenie pizzy w piecu
void push_into_the_oven(){
    // Dolaczenie segmentu pamieci wspolnej do przestrzeni adresowej procesu
    oven *oven = shmat(oven_ID, NULL, 0);

    // Losuje typ pizzy w przedziale 0-9
    pizza_TYPE = rand() % 10;

    // Przygotowanie pizzy
    print_PID_TIME(getpid(), "COOK");
    printf("Przygotowuje pizze: %d\n", pizza_TYPE);
    random_sleep(1,2);

    // Zajecie dostepu do pieca
    sembuf *sem_oven_1 = calloc(1, sizeof(sembuf));
    sem_oven_1->sem_op = -1;
    sem_oven_1->sem_num = 0;
    sem_oven_1->sem_flg = SEM_UNDO;
    semop(semaphores_SET_ID, sem_oven_1, 1);

    // Umieszczamy pizze w piecu
    for(int i = 0; i < OVEN_SIZE; i++){
        if(oven->pizza_in_oven[i] == 0){
            // Wkladamy pizze pizze
            place_in_oven = i;
            oven->pizza_in_oven[i] = 1;
            oven->pizza_type[i] = pizza_TYPE;

            // Ile pizz w piecu
            int cnt = 0;
            for( int j = 0; j < OVEN_SIZE; j++) {
                if (oven->pizza_in_oven[j] == 1){
                    cnt++;
                }
            }
                
            // Wypisujemy informacje ze wzielismy pizze danego typu
            print_PID_TIME(getpid(), "COOK");
            printf("Dodalem pizze: %d na pozycji: %d\n",pizza_TYPE ,i);
            printf("Liczba pizz w piecu: %d\n", cnt);

            break;
        }
    }


    // Zwolnienie dostepu do pieca
    sembuf *sem_oven_2 = calloc(1, sizeof(sembuf));
    sem_oven_2->sem_op = 1;
    sem_oven_2->sem_num = 0;
    sem_oven_2->sem_flg = SEM_UNDO;
    semop(semaphores_SET_ID, sem_oven_2, 1);

    // Czekamy na pizze
    random_sleep(4,5);

    // Odlaczenie segmentu pamieci
    shmdt(oven);
    // Zwolnienie zaalokowanej pamieci
    free(sem_oven_1);
    free(sem_oven_2);
}

// Wyciagniecie pizzy z pieca
void remove_from_the_oven(){
    // Dolaczenie segmentu pamieci wspolnej do przestrzeni adresowej procesu
    oven *oven = shmat(oven_ID, NULL, 0);

    // Zajecie dostepu do pieca
    sembuf *sem_oven_1 = calloc(1, sizeof(sembuf));
    sem_oven_1->sem_op = -1;
    sem_oven_1->sem_num = 0;
    sem_oven_1->sem_flg = SEM_UNDO;
    semop(semaphores_SET_ID, sem_oven_1, 1);

    // Wyciagniecie pizzy z pieca
    oven->pizza_in_oven[place_in_oven] = 0;
    
    // Ile pizz w piecu
    int cnt = 0;
    for( int i = 0; i < OVEN_SIZE; i++) {
        if (oven->pizza_in_oven[i] == 1){
            cnt++;
        }
    }

    print_PID_TIME(getpid(), "COOK");
    printf("Wyjmuje pizze: %d\n", pizza_TYPE);
    printf("Liczba pizz w piecu: %d\n", cnt);

    // Zwolnienie dostepu do pieca
    sembuf *sem_oven_2 = calloc(1, sizeof(sembuf));
    sem_oven_2->sem_op = 1;
    sem_oven_2->sem_num = 0;
    sem_oven_2->sem_flg = SEM_UNDO;
    semop(semaphores_SET_ID, sem_oven_2, 1);

    // Odlaczenie segmentu pamieci
    shmdt(oven);
    // Zwolnienie zaalokowanej pamieci
    free(sem_oven_1);
    free(sem_oven_2);
}

// Polozenie pizzy na stole
void put_on_the_table(){
    // Dolaczenie segmentu pamieci wspolnej do przestrzeni adresowej procesu
    table *table = shmat(table_ID, NULL, 0);

    // Zajmujemy dostep do stolu
    sembuf *sem_table_1 = calloc(1, sizeof(sembuf));
    sem_table_1->sem_op = -1;
    sem_table_1->sem_num = 1;
    sem_table_1->sem_flg = SEM_UNDO;
    semop(semaphores_SET_ID, sem_table_1, 1);

    for(int i = 0; i < TABLE_SIZE; i++){
        // Jesli jest miejsce na pizze na danej pozycji na stole
        if(table->pizza_on_table[i] == 0){
            // Kladziemy pizze
            table->pizza_on_table[i] = 1;
            table->pizza_type[i] = pizza_TYPE;

            // Ile pizz pozostalo na stole
            int cnt = 0;
            for( int j = 0; j < TABLE_SIZE; j++) {
                if (table->pizza_on_table[j] == 1){
                    cnt++;
                }
            }

            print_PID_TIME(getpid(), "COOK");
            printf("Klade pizze: %d na stole na pozycji: %d\n",pizza_TYPE, i);
            printf("Liczba pizz na stole: %d\n", cnt);

            break;
        }
    }

    // Zwalniamy dostep do stolu
    sembuf *sem_table_2 = calloc(1, sizeof(sembuf));
    sem_table_2->sem_op = 1;
    sem_table_2->sem_num = 1;
    sem_table_2->sem_flg = SEM_UNDO;
    semop(semaphores_SET_ID, sem_table_2, 1);

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
    shmctl(oven_ID, IPC_RMID, NULL);
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
    // Pobieramy pamieci wspolnej dla STOLU i PIECA
    get_shared_memory();

    // Wykonujemy prace kucharza
    while(1){
        push_into_the_oven();
        remove_from_the_oven();
        put_on_the_table();
    }

    return 0;
}