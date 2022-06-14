#include "common.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
client *clients[MAX_NO_CLIENTS] = {};
int no_connected_clients = 0;

int get_random_number(int lower, int upper){
    return rand() % (upper - lower + 1) + lower;
}

// Zwraca desktryptor pliku(socketu) klienta, dla ktorego zaszlo pewne zdarzenie
int get_client_descriptor(int local_server_socket, int net_server_socket){
    struct pollfd fds[2];
    // Lokalny
    fds[0].fd = local_server_socket;
    fds[0].events = POLLIN;
    // Siecowy
    fds[1].fd = net_server_socket;
    fds[1].events = POLLIN;
    poll(fds, 2, -1);
    for (int i = 0; i < 2; i++){
        if (fds[i].revents & POLLIN)
            return fds[i].fd;
    }
    printf("NIII\n");
    return -1;
}

// Zwraca index do tablicy klientow, jesli znaleziono wolne miejsce na serwerze
// -1 wpp.
int find_free_slot(){
    for(int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if(clients[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

// Zwraca index do tablicy klientow, jesli znaleziono klienta o podanej nazwie
// -1 wpp.
int find_by_name(char *name){
    for(int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if(clients[i] != NULL)
        {
            if(strcmp(clients[i]->name, name) == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

// Szukanie przeciwnika dla klienta pod indexem "index"
// -1 w przypadku niepowodzenia
// odpowiedni index do tablicy klientow wpp.
int find_opponent(int index){
    for(int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if(index != i && clients[i] != NULL)
        {
            if(clients[i]->is_in_game == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

// __QUIT__
// Usuniecie klienta z serwera
void remove_client(int index){
    free(clients[index]->name);
    free(clients[index]);
    clients[index] = NULL;
    no_connected_clients--;
}

// __GAME_OVER__
// Usuniecie klienta oraz jego przeciwnika.
void remove_both_clients(int index){
    int opponent = clients[index]->opponent;
    remove_client(index);
    if(clients[opponent] != NULL)
    {
        char buff[MAX_MSG_LENGTH];
        sprintf(buff, "quit:%s", clients[opponent]->name);
        sendto(clients[opponent]->fd, buff, MAX_MSG_LENGTH, 0, &clients[opponent]->addr, sizeof(struct sockaddr));
        remove_client(opponent);
    }
}

// Zamkniecie serwera - usuniecie wszystkich klientow(powiadomienie ich o zamknieciu serwera)
void close_server(){
    for(int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if(clients[i] != NULL)
        {
            char buff[MAX_MSG_LENGTH];
            sprintf(buff, "quit:%s", clients[i]->name);
            sendto(clients[i]->fd, buff, MAX_MSG_LENGTH, 0, &clients[i]->addr, sizeof(struct sockaddr));
            remove_client(i);
        }
    }
    exit(0);
}

// __PING__

// Usun niereagujacych uzytkownikow
void remove_offline_clients(){
    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if (clients[i] != NULL && clients[i]->is_online == 0)
        {
            remove_both_clients(i);
        }
    }
}

// Wyslij PING do uzytkownikow
void send_ping_to_all_clients(){
    for (int i = 0; i < MAX_NO_CLIENTS; i++)
    {
        if (clients[i] != NULL)
        {
            sendto(clients[i]->fd, "ping: ", MAX_MSG_LENGTH, 0, &clients[i]->addr, sizeof(struct sockaddr));
            clients[i]->is_online = 0;
        }
    }
}

// Pingowanie uzytkownikow
void ping(){
    while(1)
    {
        pthread_mutex_lock(&mutex);
        remove_offline_clients();
        send_ping_to_all_clients();
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
}

// __MAIN__

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        perror("Wrong number of arguments!\n");
        exit(0);
    }
    // Numer portu UDP 
    int port = atoi(argv[1]);

    // Sciezka gniazda UNIX
    char *socket_path = argv[2];

    printf("[SERVER] SERVER STARTED\n");

    // Stworzenie gniazda komunikacji lokalnej
    int local_server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(local_server_socket == -1)
    {
        perror("Error while creating server's local socket!\n");
        exit(0);
    }

    // Struktura sockaddr_un
    struct sockaddr_un local_server_addr;
    // Komunikacja lokalna
    local_server_addr.sun_family = AF_UNIX;
    // Sciezka do pliku, ktry reprezentuje gniazdo.
    strcpy(local_server_addr.sun_path, socket_path);

    // Zwiazanie gniazda z jego nazwa - aby moc wysylac dane do gniazda.
    int local_bind = bind(local_server_socket, (struct sockaddr *) &local_server_addr, sizeof(struct sockaddr_un));
    if(local_bind == -1)
    {
        perror("Error while binding server's local socket!\n");
        exit(0);
    }

    // Stworzenie gniazda komunikacji internetowej w oparciu o IPv4
    int net_server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(net_server_socket == -1)
    {
        perror("Error while creating server's net socket!\n");
        exit(0);
    }

    // Struktura sockaddr_in
    struct sockaddr_in net_server_addr;
    // Komunikacja internetowa
    net_server_addr.sin_family = AF_INET;
    // Numer portu
    // Za pomoca htons przeksztalcamy zwykly numer portu do "network byte order"
    net_server_addr.sin_port = htons(port);
    // Struktura opisuje dowolny adres IP posiadany przez maszynę
    net_server_addr.sin_addr.s_addr = INADDR_ANY;

    // Zwiazanie gniazda z jego nazwa- aby moc wysylac dane do gniazda.
    int net_bind = bind(net_server_socket, (struct sockaddr *) &net_server_addr, sizeof(struct sockaddr_in));
    if(net_bind == -1)
    {
        perror("Error while binding server's net socket!\n");
        exit(0);
    }

    // Obsluga sygnalu SIGINT - nalezy posprzatac
    signal(SIGINT, &close_server);

    // Watek odpowiedzialny za pingowanie uzytkownikow
    pthread_t t;
    pthread_create(&t, NULL, (void *(*)(void *))ping, NULL);

    while(1)
    {
        // Czekamy, az zajdzie pewne zdarzenie.
        int client_fd = get_client_descriptor(local_server_socket, net_server_socket);

        char buff[MAX_MSG_LENGTH];

        socklen_t length = sizeof(struct sockaddr);
        struct sockaddr addr;

        // Odbieramy dane przeslane od klienta i wyswietlamy
        recvfrom(client_fd, buff, MAX_MSG_LENGTH, 0, &addr, &length);

        // Akcja
        char *command_action = strtok(buff, ":");
        // Argumenty akcji
        char *command_args = strtok(NULL, ":");
        // Nazwa klienta
        char *client_name = strtok(NULL, ":");

        printf("[SERVER] RECEIVED FROM: %s\n", client_name);
        printf("         MESSAGE:       %s\n", command_action);
        printf("         ARGUMENTS:     %s\n\n", command_args);

        pthread_mutex_lock(&mutex);

        // akcja - initialization
        if(strcmp(command_action, "initialization") == 0)
        {
            if(find_by_name(client_name) == -1)
            {
                int client_index = find_free_slot();
                // Jesli nie ma wolnych miejsc na serwerze wysylamy wiadomosc -> no_free_slot
                if(client_index == -1)
                {
                    sendto(client_fd, "initialization:no_free_slot", MAX_MSG_LENGTH, 0, &addr, sizeof(struct addrinfo));
                }
                else
                {
                    // Dodajemy klienta do serwera
                    client *new_client = malloc(sizeof(client));
                    new_client->name = calloc(MAX_MSG_LENGTH, sizeof(char));
                    strcpy(new_client->name, client_name);
                    new_client->is_in_game = 0;
                    new_client->opponent = -1;
                    new_client->is_online = 1;
                    new_client->fd = client_fd;
                    new_client->addr = addr;
                    clients[client_index] = new_client;
                    no_connected_clients++;

                    int opponent = find_opponent(client_index);

                    // Wysylamy wiadomosc, ze nie znaleziono przeciwnika do gry -> no_enemy_found
                    if(opponent == -1)
                    {
                        sendto(client_fd, "initialization:no_enemy_found", MAX_MSG_LENGTH, 0, (struct sockaddr *) &addr, sizeof(struct addrinfo));
                    }
                    // Wysylamy wiadomosc, ze znaleziono przeciwnika do gry i odpowiedni znak, ktorym gracz bedzie gral(O\X)
                    else
                    {
                        // Ustawiamy klientom, ze sa wlasnie w grze.
                        clients[client_index]->is_in_game = 1;
                        clients[opponent]->is_in_game = 1;
                        // Ustawiamy klientom odpowiednich przeciwnikow.
                        clients[client_index]->opponent = opponent;
                        clients[opponent]->opponent = client_index;
                        
                        // Losujemy, ktory gracz zaczyna jakim znakiem.
                        int number = get_random_number(0, 1);
                        if(number % 2 == 0)
                        {
                            sendto(clients[client_index]->fd, "initialization:O", MAX_MSG_LENGTH, 0, &clients[client_index]->addr, sizeof(struct addrinfo));
                            sendto(clients[opponent]->fd, "initialization:X", MAX_MSG_LENGTH, 0, &clients[opponent]->addr, sizeof(struct addrinfo));
                        }
                        else
                        {
                            sendto(clients[opponent]->fd, "initialization:O", MAX_MSG_LENGTH, 0, &clients[opponent]->addr, sizeof(struct addrinfo));
                            sendto(clients[client_index]->fd, "initialization:X", MAX_MSG_LENGTH, 0, &clients[client_index]->addr, sizeof(struct addrinfo));
                        }
                    }
                }
            }
            // Nazwa uzytkownika jest zajeta, a zatem powiadamiamy klienta, ze nie moze miec takiej nazwy.
            else
            {
                sendto(client_fd, "initialization:username_in_use", MAX_MSG_LENGTH, 0, (struct sockaddr *) &addr, sizeof(struct addrinfo));
            }
        }
        // akcja - move - ruch klienta w grze
        else if(strcmp(command_action, "move") == 0)
        {
            int move_option = atoi(command_args);
            int player = find_by_name(client_name);
            int opponent = clients[player]->opponent;
            sprintf(buff, "opponent_move:%d", move_option);
            sendto(clients[opponent]->fd, buff, MAX_MSG_LENGTH, 0, &clients[opponent]->addr, sizeof(struct addrinfo));
        }
        // akcja - quit - klient opuszcza serwer
        else if(strcmp(command_action, "quit") == 0)
        {
            int player = find_by_name(client_name);
            remove_both_clients(player);
        }
        // akcja - game_over - gra sie zakonczyla, usuwamy obu graczy
        else if(strcmp(command_action, "game_over") == 0)
        {
            int player = find_by_name(client_name);
            remove_client(player);
        }
        // akcja - ping - odpowiedz klienta na ping
        else if(strcmp(command_action, "ping") == 0)
        {
            int player = find_by_name(client_name);
            clients[player]->is_online = 1;
        }

        pthread_mutex_unlock(&mutex);
    }
    
    return 0;
}