#include "common.h"

sem_t* semaphore_TABLE;
sem_t* semaphore_OVEN;

int fd_TABLE;
int fd_OVEN;

int pizza_TYPE;
int place_in_oven;

// Pobieranie identyfikatora istniejacych juz semaforow
// tworzonych przez proces main'a.
void get_semaphores(){
    // Pobieramy semafor stolu
    semaphore_TABLE = sem_open("/TABLE", O_RDWR);

    if(semaphore_TABLE == SEM_FAILED){
        perror("Error while opening table's semaphore in COCK!");
        exit(EXIT_FAILURE);
    }

    // Pobieramy semafor pieca
    semaphore_OVEN = sem_open("/OVEN", O_RDWR);

    if(semaphore_OVEN == SEM_FAILED){
        perror("Error while opening oven's semaphore in COCK!");
        exit(EXIT_FAILURE);
    }
}

// Pobieramy pamieci wspolnych
void get_shared_memory() {
    // Pobranie segmentu pamieci wspolnej dla stolu
    fd_TABLE = shm_open(TABLE_SHARED_MEMORY, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if(fd_TABLE == -1){
        perror("Error while opening memory segment for TABLE in COOK!");
        exit(EXIT_FAILURE);
    }

    // Pobranie segmentu pamieci wspolnej dla pieca
    fd_OVEN = shm_open(OVEN_SHARED_MEMORY, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if(fd_OVEN == -1){
        perror("Error while opening memory segment for OVEN in COOK!");
        exit(EXIT_FAILURE);
    }
}

// Umieszczenie pizzy w piecu
void push_into_the_oven(){
    // Dolaczenie segmentu pamieci wspolnej pieca do przestrzeni adresowej procesu
    oven* oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, fd_OVEN, 0);

    if(oven == (void*) -1){
       perror("Error while opening memory segment for OVEN in COOK!");
         exit(EXIT_FAILURE);
     }

    // Losuje typ pizzy w przedziale 0-9
    pizza_TYPE = rand() % 10;

    // Przygotowanie pizzy
    print_PID_TIME(getpid(), "COOK");
    printf("Przygotowuje pizze: %d\n", pizza_TYPE);
    random_sleep(1,2);

    int flag = 0;
    while(flag == 0){

        // Zajecie dostepu do pieca
        if(sem_wait(semaphore_OVEN) == -1){
        perror("Error while waiting for OVEN in COOK!");
        exit(EXIT_FAILURE);
        }

        // Umieszczamy pizze w piecu
        for(int i = 0; i < OVEN_SIZE; i++){
            if(oven->pizza_in_oven[i] == 0){
            // Wkladamy pizze
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
                
            // Wypisujemy informacje ze umiescilismy pizze danego typu w piecu
            print_PID_TIME(getpid(), "COOK");
            printf("Dodalem pizze: %d na pozycji: %d\n",pizza_TYPE ,i);
            printf("Liczba pizz w piecu: %d\n", cnt);
            
            flag = 1;
            break;
            }
        }

        // Zwalniamy dostep do pieca
        if(sem_post(semaphore_OVEN) == -1){
        perror("Error while posting for OVEN in COOK!");
        exit(EXIT_FAILURE);
        }
    }

    // Czekamy na pizze
    random_sleep(4,5);

    // Odlaczamy segment pamieci dla pieca
    munmap(oven, sizeof(oven));
}

// Wyciagniecie pizzy z pieca
void remove_from_the_oven(){
    // Dolaczamy segmentu pamieci wspolnej dla pieca do przestrzeni adresowej procesu
    oven* oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, fd_OVEN, 0);

    if(oven == (void*) -1){
       perror("Error while opening memory segment for OVEN in COOK!");
         exit(EXIT_FAILURE);
     }
    
    // Zajmujemy dostepu do pieca
    if(sem_wait(semaphore_OVEN) == -1){
        perror("Error while waiting for OVEN in COOK!");
        exit(EXIT_FAILURE);
    }

    // Wyciagamy pizze z pieca
    oven->pizza_in_oven[place_in_oven] = 0;
    
    // Ile pizz w piecu
    int cnt = 0;
    for( int i = 0; i < OVEN_SIZE; i++) {
        if (oven->pizza_in_oven[i] == 1){
            cnt++;
        }
    }

    // Wypisujemy informacje, ze wyjmujemy pizze z pieca
    print_PID_TIME(getpid(), "COOK");
    printf("Wyjmuje pizze: %d\n", pizza_TYPE);
    printf("Liczba pizz w piecu: %d\n", cnt);

    // Zwalniamy dostep do pieca
    if(sem_post(semaphore_OVEN) == -1){
        perror("Error while posting for OVEN in COOK!");
        exit(EXIT_FAILURE);
    }

    // Odlaczamy segment pamieci
    munmap(oven, sizeof(oven));
}

// Polozenie pizzy na stole
void put_on_the_table(){
    // Dolaczamy segmentu pamieci wspolnej dla stolu do przestrzeni adresowej procesu
    table* table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, fd_TABLE, 0);

    if(table == (void*) -1){
       perror("Error while opening memory segment for TABLE in DELIVERER!");
         exit(EXIT_FAILURE);
     }


    int flag = 0;
    while(flag == 0){

        // Zajmujemy dostep do stolu
        if(sem_wait(semaphore_TABLE) == -1){
        perror("Error while waiting for TABLE in DELIVERER!");
        exit(EXIT_FAILURE);
        }

        // Umieszczamy pizze na wolnym miejscu na stole
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

                // Wypisujemy informacje ze polozylismy pizze danego typu na stole
                print_PID_TIME(getpid(), "COOK");
                printf("Klade pizze: %d na stole na pozycji: %d\n",pizza_TYPE, i);
                printf("Liczba pizz na stole: %d\n", cnt);

                flag = 1;
                break;
            }
        }

        // Zwalniamy dostep do stolu
        if(sem_post(semaphore_TABLE) == -1){
            perror("Error while posting FOR TABLE in DELIVERER!");
            exit(EXIT_FAILURE);
        }
    }

    // Odlaczamy segmentu pamieci
    munmap(table, sizeof(table));
}

// W przypadku otrzymania sygnalu SIGINT musze
// usunac zamknac semafory
void handler(){
    // Zamykamy semafory
    sem_close(semaphore_TABLE);
    sem_close(semaphore_OVEN);
    
    // Odlaczamy segmenty pamieci
    shm_unlink(OVEN_SHARED_MEMORY);
    shm_unlink(TABLE_SHARED_MEMORY);

    // Konczymy dzialanie programu
    exit(0);
}

int main(int argc, char** argv){
    // Obsluga sygnalu SIGINT -> trzeba posprzatac
    signal(SIGINT, handler);

    // Pobieramy zbior semaforow
    get_semaphores();
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