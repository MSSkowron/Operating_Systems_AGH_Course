#include "common.h"

char *client_name;
char *connection_type;
char *command_args;
int server_socket;
board_field client_field_sign;
game_status current_game_status = GAME_NOT_STARTED;
board game_board;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Zwraca identyfikator pliku(socket) serwera Lokalnego
int connect_local(char *path){
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Error while creating local socket\n");
        exit(0);
    }

    // Struktura sockaddr_un
    struct sockaddr_un local_addr;
    // Komunikacja lokalna
    local_addr.sun_family = AF_UNIX;
    // Sciezka do pliku, ktory reprezentuje gniazdo
    strcpy(local_addr.sun_path, path);

    // Polaczenie klienta z serwerem
    int client_connection = connect(server_socket, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_un));
    if(client_connection == -1){
        perror("Error while connecting to local server\n");
        exit(0);
    }
    return server_socket;
}

// Zwraca identyfikator pliku(socket) serwera Sieciowego
int connect_inet(int port){
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Error while creating net socket\n");
        exit(0);
    }

    // Struktura sockaddr_in
    struct sockaddr_in net_addr;
    // Komunikacja sieciowa
    net_addr.sin_family = AF_INET;
    // Za pomoca htons przeksztalcamy zwykly numer portu do "network byte order"
    net_addr.sin_port = htons(port);
    // Struktura opisuje dowolny adres IP posiadany przez maszynę    
    net_addr.sin_addr.s_addr = INADDR_ANY;

    // Polaczenie klienta z serwerem
    int client_connection = connect(server_socket, (struct sockaddr *) &net_addr, sizeof(struct sockaddr_in));
    if(client_connection == -1){
        perror("Error while connecting to net server\n");
        exit(0);
    }
    return server_socket;
}

// Stworzenie nowej planszy do gry
board create_new_board(){
    board new_board = {{EMPTY}};
    return new_board;
}

// Rysujemy plansze z odpowiednimi wartosciami pol
void draw_board(){
    printf("\n");
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf("|");
            if(game_board.fields[3 * i + j] == X)
                printf("X");
            else if(game_board.fields[3 * i + j] == O)
                printf("O");
            else
                printf(" ");
        }
        printf("|\n");
    }
    printf("\n");
}

// Sprawdzamy, czy gracz nie wybral wartosci z poza planszy oraz pola, ktore jest juz zajete
// jesli tak, to probuje ponownie.
int check_move(int move){
    if(move >= 0 && move <= 8 && game_board.fields[move] == EMPTY)
    {
        return 1;
    }
    printf("Wrong move, try again!\n");
    return 0;
}

// Sprawdzamy, czy gra sie juz zakonczyla
int check_end(board_field sign){
    for(int i = 0; i < 3; i++){
        int row_win = 1;
        for(int j = 0; j < 3; j++){
            if(game_board.fields[i * 3 + j] != sign){
                row_win = 0;
                break;
            }
        }
        if(row_win == 1)
            return 1;
    }
    for(int j = 0; j < 3; j++){
        int column_win = 1;
        for(int i = 0; i < 3; i++){
            if(game_board.fields[i * 3 + j] != sign){
                column_win = 0;
                break;
            }
        }
        if(column_win == 1)
            return 1;
    }
    int cross_win = 1;
    for(int i = 0; i < 9; i += 4){
        if(game_board.fields[i] != sign){
            cross_win = 0;
            break;
        }
    }
    if(cross_win == 1)
        return 1;
    cross_win = 1;
    for(int i = 2; i < 7; i += 2){
        if(game_board.fields[i] != sign){
            cross_win = 0;
            break;
        }
    }
    if(cross_win == 1)
        return 1;
    for(int i = 0; i < 9; i++){
        if(game_board.fields[i] == EMPTY)
            return 0;
    }
    return -1;
}

// Zamykamy polaczenie z serwerem
void close_connection(){
    int shut = shutdown(server_socket, SHUT_RDWR);
    if(shut == -1){
        perror("Error while closing connection with server\n");
        exit(0);
    }
    int closed = close(server_socket);
    if(closed == -1){
        perror("Error while closing file(socket) server descriptor\n");
        exit(0);
    }
    exit(0);
}

// Wyjscie z serwera
void quit(){
    char buff[MAX_MSG_LENGTH];
    sprintf(buff, "quit: :%s", client_name);
    send(server_socket, buff, MAX_MSG_LENGTH, 0);
    close_connection();
}

// Zakonczenie gry
void game_over(){
    char buff[MAX_MSG_LENGTH];
    sprintf(buff, "game_over: :%s", client_name);
    send(server_socket, buff, MAX_MSG_LENGTH, 0);
    close_connection();
}

// Dla O -> X
// Dla X -> O
board_field get_opposite_sign(board_field sign){
    return sign == O ? X : O;
}

// Gra
void play_game(){
    while (1)
    {
        if (current_game_status == GAME_NOT_STARTED)
        {
            // Nazwa uzytkownika jest juz zajeta wiadomosc -> username_in_use
            if (strcmp(command_args, "username_in_use") == 0)
            {
                printf("Username already in use\n");
                exit(0);
            }
            // Nie znaleziono przeciwnika wiadomosc -> enemy_no_found
            else if (strcmp(command_args, "no_enemy_found") == 0)
            {
                printf("Wating for an opponent\n");
                current_game_status = LOOKING_FOR_OPPONENT;
            }
            // Brak miejsca na serwerze wiadomosc -> no_free_slot
            else if (strcmp(command_args, "no_free_slot") == 0)
            {
                printf("There is no free slot on the server\n");
                exit(0);
            }
            // Rozpoczecie gry wiadomosc -> initialization:0 lub initialization:X
            else
            {
                // Tworzymy plansze
                game_board = create_new_board();

                // Ja zaczynam
                if(command_args[0] == 'O')
                {
                    client_field_sign = O;
                    current_game_status = MY_TURN;
                }
                // Przeciwnik zaczyna
                else
                {
                    client_field_sign = X;
                    current_game_status = OPPONENTS_TURN;
                }
                // Rysujemy plansze poczatkowa
                draw_board();
            }
        }
        // Oczekiwanie na znalezienie przeciwnika
        else if (current_game_status == LOOKING_FOR_OPPONENT)
        {
            // Czekamy, dopoki nie znajdzie nam przeciwnika
            pthread_mutex_lock(&mutex);
            while (current_game_status != GAME_NOT_STARTED && current_game_status != GAME_OVER)
            {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
            
            // Znaleziono przeciwnika
            // Rysujemy plansze
            game_board = create_new_board();

            // Ja zaczynam
            if(command_args[0] == 'O')
            {
                client_field_sign = O;
                current_game_status = MY_TURN;
            }
            // Przeciwnik zaczyna
            else
            {
                client_field_sign = X;
                current_game_status = OPPONENTS_TURN;
            }
            // Rysujemy plansze poczatkowa
            draw_board();
        }
        // Oczekiwanie na wykonanie ruchu przez przeciwnika
        else if (current_game_status == OPPONENTS_TURN)
        {
            printf("Waiting till opponent makes a move\n");

            pthread_mutex_lock(&mutex);
            while (current_game_status != OPPONENTS_TURN_FINISHED && current_game_status != GAME_OVER){
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        // Moj ruch
        else if (current_game_status == MY_TURN)
        {
            int move;
            int move_result;
            while(1){
                printf("Your turn [%c]: ", client_field_sign == O ? 'O' : 'X');
                scanf("%d", &move);
                move--;
                if(check_move(move) == 1)
                {
                    break;
                }
            }
            // Aktualizacja planszy
            game_board.fields[move] = client_field_sign;
            draw_board();

            // Wyslanie wiadomosci do serwa o ruchu, ktory wykonuje
            char buff[MAX_MSG_LENGTH];
            sprintf(buff, "move:%d:%s", move, client_name);
            send(server_socket, buff, MAX_MSG_LENGTH, 0);

            // Sprawdzamy, czy gra sie zakonczyla
            int end = check_end(client_field_sign);
            if(end != 0)
            {
                if(end == 1)
                {
                    printf("You WON! :)\n");
                }
                else
                {
                    printf("DRAW!\n");
                }
                current_game_status = GAME_OVER;
            }
            if (current_game_status != GAME_OVER)
            {
                current_game_status = OPPONENTS_TURN;                
            }
        }
        // Przeciwnik wykonal ruch
        else if (current_game_status == OPPONENTS_TURN_FINISHED)
        {
            // Aktualizacja tablicy
            int move = atoi(command_args);
            board_field sign = get_opposite_sign(client_field_sign);
            game_board.fields[move] = sign;
            draw_board();

            // Sprawdzamy, czy gra sie zakonczyla
            int end = check_end(sign);
            if(end != 0)
            {
                if(end == 1)
                {
                    printf("You LOST! :(\n");
                }

                else
                {
                    printf("DRAW!\n");
                }
                current_game_status = GAME_OVER;
            }
            if (current_game_status != GAME_OVER)
            {
                current_game_status = MY_TURN;
            }

        }
        // Gra sie zakonczyla
        else if (current_game_status == GAME_OVER)
        {
            game_over();
        }
    }
}

// Wysylamy odpowiedz na ping do serwera
void ping(){
    char buff[MAX_MSG_LENGTH];
    sprintf(buff, "ping: :%s", client_name);
    send(server_socket, buff, MAX_MSG_LENGTH, 0);
}

int main(int argc, char** argv){
    if(argc != 4){
        perror("Wrong number of arguments!\n");
        exit(0);
    }
    // Nazwa klienta
    client_name = argv[1];

    // Sposob polaczenia z serwerem
    connection_type = argv[2];

    // Komunikacja lokalna
    if(strcmp(connection_type, "unix") == 0)
    {
        char *server_socket_path = argv[3];
        server_socket = connect_local(server_socket_path);
    }
    // Komunikacja sieciowa
    else if(strcmp(connection_type, "inet") == 0)
    {
        int server_port = atoi(argv[3]);
        server_socket = connect_inet(server_port);
    }
    else{
        perror("Wrong communication type.\n Available types: unix, inet\n");
        exit(0);
    }

    signal(SIGINT, &quit);

    // Wiadomosc do serwera o dolaczeniu.
    char msg[MAX_MSG_LENGTH];
    sprintf(msg, "initialization: :%s", client_name);
    send(server_socket, msg, MAX_MSG_LENGTH, 0);

    char buff[MAX_MSG_LENGTH];
    char *command_action;
    int game_thread_running = 0;
    while(1)
    {
        // Odebranie wiadomosci od serwera
        recv(server_socket, buff, MAX_MSG_LENGTH, 0);

        // Akcja
        command_action = strtok(buff, ":");
        // Argumenty akcji
        command_args = strtok(NULL, ":");
        
        pthread_mutex_lock(&mutex);
    
        if(strcmp(command_action, "initialization") == 0)
        {
            current_game_status = GAME_NOT_STARTED;
            // Zaczynamy gre
            if (game_thread_running == 0)
            {
                pthread_t t;
                int thread = pthread_create(&t, NULL, (void *(*)(void *))play_game, NULL);
                if(thread != 0)
                {
                    perror("Error while thread creating game \n");
                    exit(0);
                }
                game_thread_running = 1;
            }
        }
        // Ruch przeciwnika
        else if(strcmp(command_action, "opponent_move") == 0)
        {
            current_game_status = OPPONENTS_TURN_FINISHED;
        }
        // Opuszczenie serwera
        else if(strcmp(command_action, "quit") == 0)
        {
            current_game_status = GAME_OVER;
            close_connection();
        }
        // Ping
        else if(strcmp(command_action, "ping") == 0)
        {
            ping();
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}