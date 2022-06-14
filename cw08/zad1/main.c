#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

int no_threads;
int height;
int width;
int** input_matrix;
int** output_matrix;


void get_time(struct timespec start, struct timespec end, double *time_value){
    int result = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    *time_value = 1.0 * result/1000000.0;
}

// Odczytuje plik i zapisuje go do macierzy
void read_input(char* filename){
    FILE* file = fopen(filename, "r");
    if(file == NULL){
        perror("Cannot read the input file");
        exit(1);
    }
    
    // Naglowki
    char buffer[10];
    fscanf(file, "%s", buffer);
    fscanf(file, "%d %d", &width, &height);
    fscanf(file, "%d", (int*) buffer);

    // Pamiec dla macierzy wejsciowej oraz wyjsciowej
    input_matrix = calloc(height, sizeof(int*));
    output_matrix = calloc(height, sizeof(int*));
    for(int i = 0; i < height; i++){
        input_matrix[i] = calloc(width, sizeof(int));
        output_matrix[i] = calloc(width, sizeof(int));
    }

    // Macierzy wejsciowa
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fscanf(file, "%d", &input_matrix[i][j]);
        }
    }

    fclose(file);
}

// Zapisuje macierz wyjsciowa do pliku
void write_result_to_file(char* filename){
    FILE* file = fopen(filename, "w");
    if(file == NULL){
        puts("File opening failed.");
        exit(1);
    }

    fprintf(file, "%s\n", "P2");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", 255);

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            fprintf(file, "%d ", output_matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}


// Tryb numbers -> po rowno
void numbers_mode(int thread_id){
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(input_matrix[i][j] % no_threads == thread_id){
                output_matrix[i][j] = 255 - input_matrix[i][j];
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);

    double *time_val = malloc(sizeof (double ));
    get_time(start, end, time_val);

    pthread_exit(time_val);
}


// Tryb block -> x'owo od (k - 1) * ceil(N/m) do k * ceil(N/m) - 1
void block_mode(int k){
    struct timespec start, end;

    int left_range = (k-1)*ceil(width/no_threads);
    int right_range = k*ceil(width/no_threads) - 1;

    clock_gettime(CLOCK_REALTIME, &start);

    for(int i = 0; i < height; i++){
        for(int j = left_range; j <= right_range; j++){
            output_matrix[i][j] = 255 - input_matrix[i][j];
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);

    double *time_val = malloc(sizeof (double ));
    get_time(start, end, time_val);

    pthread_exit(time_val);
}


int main(int argc, char** argv){
    if(argc != 5) {
        perror("Invalid number of arguments");
        exit(1);
    }

    // 1. liczba watków
    no_threads = atoi(argv[1]);

    // 2. tryb numbers/block
    char mode[20];
    strcpy(mode, argv[2]);

    // 3.nazwa plik wejsciowego z obrazem *.pgm
    char input_filename[100];
    strcpy(input_filename, argv[3]);

    // 4.nazwa pliku wynikowego
    char output_filename[100];
    strcpy(output_filename, argv[4]);

    printf("Number of threads: %d \nMode: %s\n", no_threads, mode);

    // Odczytanie macierzy wejsciowej
    // input_matrix  - macierz wejsciowa
    // output_matrix - macierz wyjsciowa
    read_input(input_filename);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Pamiec dla ID watków
    pthread_t* threads_set = calloc(no_threads, sizeof(pthread_t));

    // Trybu pracy (numbers lub block) oraz utworzenie watków
    if(strcmp(mode, "numbers") == 0){
        for(int i = 0; i < no_threads; i++){
            pthread_create(&threads_set[i], NULL, (void*)numbers_mode, i);
        }
    }
    else if(strcmp(mode, "block") == 0){
        for(int i = 0; i < no_threads; i++){
            pthread_create(&threads_set[i], NULL, (void*)block_mode, i+1);
        }
    }
    else{
        perror("Invalid mode type");
        exit(1);
    }

    // Czas pracy poszczególnych watków - zwracany za pomoca pthread_exit()
    for(int i = 0; i < no_threads; i++){
        double *result;
        pthread_join(threads_set[i], (void*)&result);
        printf("Thread: %d     time: %lf\n", i, *result);
    }

    clock_gettime(CLOCK_REALTIME, &end);

    // Czas pracy calego programu
    double *time_val = malloc(sizeof (double ));
    get_time(start, end, time_val);
    printf("Main          time: %lf\n", *time_val);

    // Zapis wynikow do pliku wyjsciowego
    write_result_to_file(output_filename);

    // Zwalnianie pamieci
    for(int i = 0; i < height; i++){
        free(input_matrix[i]);
        free(output_matrix[i]);
    }
    free(input_matrix);
    free(output_matrix);
    free(threads_set);
    return 0;
}