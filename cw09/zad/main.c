#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define REINDEER_NUM 9
#define ELVES_NUM 10
#define ELVES_WAITING_FOR_SANTA 3

#define ELF_WORKING_TIME rand() % 4 + 2             // 2-5s
#define SANTA_SOLVING_PROBLEM_TIME rand() % 2 + 1   // 1-2s
#define DELIVERING_TOYS_TIME rand() % 3 + 2         // 2-4s
#define REINDEER_HOLIDAYS_TIME rand() % 6 + 5       // 5-10s

// Ile reniferow powrocilo z wakacji
int no_reindeer_back = 0;
// Numer dostawy
int no_delivery = 0;
// Renifer/y czekaja na mikolaja
bool reindeer_are_waiting = false;
// Ile elfow czekaja na mikolaja
int no_elves_waiting_for_santa = 0;
// Kolejka przed wartsztatem czekajaca az mikolaj im pomoze :(
int elves_queue[ELVES_WAITING_FOR_SANTA];

                                                                // MIKOLAJ:
pthread_mutex_t mutex_santa = PTHREAD_MUTEX_INITIALIZER;        // blokada na mikolaja
pthread_cond_t cond_santa   = PTHREAD_COND_INITIALIZER;         // zmienna warunkowa mikolaja

                                                                 // RENIFERY:
pthread_mutex_t mutex_reindeer = PTHREAD_MUTEX_INITIALIZER;      // blokada na renifery 
pthread_mutex_t mutex_reindeer_wait = PTHREAD_MUTEX_INITIALIZER; // blokada na renifery oczekujace
pthread_cond_t cond_reindeer_wait   = PTHREAD_COND_INITIALIZER;  // zmienna warunkowa reniferow

                                                                 // ELFY:
pthread_mutex_t mutex_elves = PTHREAD_MUTEX_INITIALIZER;         // blokada na elfy  
pthread_mutex_t mutex_elves_wait = PTHREAD_MUTEX_INITIALIZER;    // blokada na elfy oczekujace
pthread_cond_t cond_elves_wait   = PTHREAD_COND_INITIALIZER;     // zmienna warunkowa elfow


void reset_queue(int* queue){
    for (int i = 0; i < ELVES_WAITING_FOR_SANTA; i++){
        queue[i] = -1;
    }
}


void* santa(void* arg){

    while (true){
        // Usypiamy watek dopoki nie zajdzie jeden z dwoch warunkow - (3 elfy lub 9 reniferow)
        pthread_mutex_lock(&mutex_santa);

        while (no_elves_waiting_for_santa < ELVES_WAITING_FOR_SANTA && no_reindeer_back < REINDEER_NUM){
            printf("\nMikolaj: Spie, Elfy: (%d), Renifery (%d)\n", no_elves_waiting_for_santa, no_reindeer_back);
            pthread_cond_wait(&cond_santa, &mutex_santa);
        }

        pthread_mutex_unlock(&mutex_santa);

        printf("\nMikołaj: Budzę się, Elfy: (%d), Renifery (%d)\n", no_elves_waiting_for_santa, no_reindeer_back);

        // W pierwszej kolejnosci sprawdzamy czy wszystkie renifery wrocily
        // jesli tak, to dostarczamy zabawki
        pthread_mutex_lock(&mutex_reindeer);

        if (no_reindeer_back == REINDEER_NUM) {

            no_delivery++;
            printf("\nMikołaj: Dostarczam zabawki. Numer dostawy: %d\n\n", no_delivery);
            sleep(DELIVERING_TOYS_TIME);
            no_reindeer_back = 0;

            pthread_mutex_lock(&mutex_reindeer_wait);

            reindeer_are_waiting = false;
            pthread_cond_broadcast(&cond_reindeer_wait);

            pthread_mutex_unlock(&mutex_reindeer_wait);

        }
        pthread_mutex_unlock(&mutex_reindeer);

        // Zakladamy, ze mikolaj moze dostarczyc prezenty tylko 3 razy
        if (no_delivery == 3) {
            printf("\nMikolaj: Dostarczylem wszystkie prezenty, koncze watki\n\n");
            break;
        }

        // Jesli na mikolaja czekaja elfy to pomaga im rozwiazac problem.
        pthread_mutex_lock(&mutex_elves);
        if (no_elves_waiting_for_santa == ELVES_WAITING_FOR_SANTA) {
            printf("\nMikołaj: Rozwiązuje problemy elfów o ID: (%d, %d, %d)\n\n", elves_queue[0], elves_queue[1], elves_queue[2]);
            sleep(SANTA_SOLVING_PROBLEM_TIME);

            pthread_mutex_lock(&mutex_elves_wait);
            reset_queue(elves_queue);                  // Zerujemy tablice zawierajaca ID elfow z koleiki.
            no_elves_waiting_for_santa = 0;            // Teraz czeka 0 elfow.
            pthread_cond_broadcast(&cond_elves_wait);  // Powiadamiamy wszystkie czekajace elfy na warsztat.
            pthread_mutex_unlock(&mutex_elves_wait);

        }

        printf("\nMikołaj: Zasypiam\n");
        pthread_mutex_unlock(&mutex_elves);
    }

    exit(0);
}

void* reindeer(void* arg){
    int ID = *((int *) arg);

    while(true){
        // Jesli renifery oczekuja na mikolaja to czekaja.
        pthread_mutex_lock(&mutex_reindeer_wait);
        while (reindeer_are_waiting) {
            pthread_cond_wait(&cond_reindeer_wait, &mutex_reindeer_wait);
        }
        pthread_mutex_unlock(&mutex_reindeer_wait);

        sleep(REINDEER_HOLIDAYS_TIME);

        pthread_mutex_lock(&mutex_reindeer);
        no_reindeer_back++;
        printf("Renifer: Czeka %d reniferów na Mikołaja, ID: %d\n", no_reindeer_back, ID);
        reindeer_are_waiting = true;

        if (no_reindeer_back == REINDEER_NUM) {
            printf("Renifer: Wybudzam Mikołaja, ID: %d\n\n", ID);
            pthread_mutex_lock(&mutex_santa);
            pthread_cond_broadcast(&cond_santa); // Wyslij powiadomienie Mikolajowi, ze wszystkie renifery wrocily z wakacji. 
            pthread_mutex_unlock(&mutex_santa);

        }
        pthread_mutex_unlock(&mutex_reindeer);
    }
}


void* elf(void* arg){
    int ID = *((int *) arg);

    while(true){
        sleep(ELF_WORKING_TIME);

        // Czekaj dopoki nie zwolni sie miejsce przed warsztatem(jesli stoi tam wymagana ilosc elfow(domyslnie 3)).
        pthread_mutex_lock(&mutex_elves_wait);

        while (no_elves_waiting_for_santa == ELVES_WAITING_FOR_SANTA) {
            printf("Elf: czeka na powrót elfów, ID: %d\n", ID);
            pthread_cond_wait(&cond_elves_wait, &mutex_elves_wait);
        }

        pthread_mutex_unlock(&mutex_elves_wait);

        pthread_mutex_lock(&mutex_elves);

        // Idz przed warsztat.
        if (no_elves_waiting_for_santa < ELVES_WAITING_FOR_SANTA){
            elves_queue[no_elves_waiting_for_santa] = ID;
            no_elves_waiting_for_santa++;
            printf("Elf: Czeka %d elfów na Mikołaja, ID: %d\n", no_elves_waiting_for_santa, ID);

            if (no_elves_waiting_for_santa == ELVES_WAITING_FOR_SANTA){
                printf("Elf: Wybudzam Mikołaja, ID: %d\n", ID);
                pthread_mutex_lock(&mutex_santa);
                pthread_cond_broadcast(&cond_santa); // Wyslij powiadomienie Mikolajowi, zeby wstal i pomogl :(
                pthread_mutex_unlock(&mutex_santa);
            }
        }

        pthread_mutex_unlock(&mutex_elves);
    }
}


int main(){
    srand(time(NULL));

    reset_queue(elves_queue);

    // Watek mikolaja
    pthread_t santa_t;
    pthread_create(&santa_t, NULL, &santa, NULL);

    // Watki reniferow
    int* reindeer_ID = calloc(REINDEER_NUM, sizeof(int));
    pthread_t* reindeer_t = calloc(REINDEER_NUM, sizeof(pthread_t));
    for (int i = 0; i < REINDEER_NUM; i++){
        reindeer_ID[i] = i;
        pthread_create(&reindeer_t[i], NULL, &reindeer, &reindeer_ID[i]);
    }

    // Watki elfow
    int* elves_ID = calloc(ELVES_NUM, sizeof(int));
    pthread_t* elves_t = calloc(ELVES_NUM, sizeof(pthread_t));
    for (int i = 0; i < ELVES_NUM; i++){
        elves_ID[i] = i;
        pthread_create(&elves_t[i], NULL, &elf, &elves_ID[i]);
    }


    // Oczekiwanie na zakonczenie watkow.
    pthread_join(santa_t, NULL);
    for (int i = 0; i < REINDEER_NUM; i++){
        pthread_join(reindeer_t[i], NULL);
    }
    for (int i = 0; i < ELVES_NUM; i++){
        pthread_join(elves_t[i], NULL);
    }

    return 0;
}