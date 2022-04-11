#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE_SIZE 256
#define MAX_NO_COMMANDS 10
#define MAX_NO_ARGUMENTS 5
#define MAX_COMMAND_LENGTH 1000

// Szukam w pliku definicji argumentu np. dla skladnik4 => "cat /etc/passwd | wc -l"
void find_definition_in_file(char* definition_of_element, char* element, FILE* file){
    fseek(file, 0, 0);
    int n = strlen(element);
    char* file_fragment = calloc(n, sizeof(char));
    fread(file_fragment, sizeof(char), n, file);
    while(strcmp(file_fragment, element) != 0){
        fseek(file, -n+1, 1);
        fread(file_fragment, sizeof(char), n, file);
    }

    // Przeskakuje za znak rownosci, ustawiam sie na poczatku ciagu komend.
    fseek(file, 3, 1);
    // Do zmiennej definition_of_element wczytuje jej definicje np. "cat /etc/passwd | wc -l"
    fgets(definition_of_element, MAX_COMMAND_LENGTH, file);

    free(file_fragment);
}

// Parsowanie komendy np. "skladnik4" => "cat /etc/passwd | wc -l"
void parse_command(char* non_parsed, char* parsed, FILE* file){
    int prev = 0;
    for(int curr = 0; curr < strlen(non_parsed); curr++){
        if(non_parsed[curr] == '\n' || non_parsed[curr] == ' ')
        {
            // element -> skladnikX
            // prev    -> poczatek elementu
            // curr    -> koniec elementu
            char* element = calloc(curr-prev-1, sizeof(char));
            int i = 0;
            for(int j = prev; j < curr; j++){
                element[i] = non_parsed[j];
                i++;
            }

            // [definition_of_element] = np. "cat /etc/passwd | wc -l"
            char* definition_of_element = calloc(MAX_COMMAND_LENGTH, sizeof(char));
            find_definition_in_file(definition_of_element, element, file);

            // Dodaje ciag komend do naszego wyniku.
            strcat(parsed, definition_of_element);
            strtok(parsed, "\n");

            // Przeskakuje na kolejny skladnik w ciagu
            curr += 3;
            prev = curr;

            // Jesli nie doszedlem do konca tzn. ze bedzie kolejna komenda w potoku,
            // wiec nalezy dodac "|".
            if(curr < strlen(non_parsed)) 
            {
                strcat(parsed, " | ");
            }

            free(element);
            free(definition_of_element);
        }
    }
}

// np. "cat /etc/passwd | wc -l" => 2, bo 1 - "cat/etc/passwd", 2 - "wc -l"
int count_separated_commands(char* command){
    int counter = 1;
    for(int i = 0; i < strlen(command); i++){
        if (command[i] == '|') counter++;
    }
    return counter;
}

// dostajemy komende np. "cat /etc/passwd | wc -l" i zakres, ktory chcemy sparsowac.
// np. naszym zakresem jest "cat /etc/passwd |"
// command_name -> cat
// arguments    -> /etc/passwd
void parse_to_name_and_arguments(char* command, char* command_name, char** arguments, int from, int to){
    int argument_index = 0;    //index w tablicy arguments
    int prev = from;           //lewy wskaznik
    int curr;                  //prawy wskaznik

    // Pierwsze pobieram nazwe komendy np. cat lub wc. => command_name
    for(curr = from; curr <= to; curr++)
    {
        if(curr == to || command[curr] == ' ')
        {
            for(int j = prev; j < curr; j++)
            {
                char* sign = calloc(1, sizeof(char));
                sign[0] = command[j];
                strcat(command_name, sign);
                free(sign);
            }
            argument_index += 1;
            curr++;
            prev = curr;
            break;
        }
    }
    
    // Teraz przechodze dalej pobierajac kolejne argumenty dla naszej komendy. => arguments
    while (curr <= to)
    {
        if(curr == to || command[curr] == ' ')
        {
            for(int j = prev; j < curr; j++)
            {
                char* sign = calloc(1, sizeof(char));
                sign[0] = command[j];
                strcat(arguments[argument_index], sign);
                free(sign);
            }
            argument_index += 1;
            prev = curr + 1;
        }
        curr++;
    }

    if(to == strlen(command)) 
    {
        arguments[argument_index] = NULL;
    }
    else
    {
        arguments[argument_index-1] = NULL;
    }
}

// Dostajemy sparsowana komende do wykonania np. "cat /etc/passwd | wc -l"
void execute_command(char* command){
    printf("[EXECUTING COMMAND]: %s\n",command);
    int n = strlen(command);
    int no_separated_commands = count_separated_commands(command);
    int curr_ptr = 0;
    int prev_ptr = 0;

    for(int i = 0; i < no_separated_commands; i++)
    {
        // Lapiemy oddzielna komende np. "cat /etc/passwd |" lub "wc -l", gdy koniec.
        // prev_ptr - wskazuje na poczatek
        // curr_ptr wskazuje na "|" lub koniec.
        while(curr_ptr < n && command[curr_ptr] != '|')
        {
            curr_ptr++;
        }

        // Nazwa komendy np. cat lub wc.
        char* command_name = calloc(50, sizeof(char));

        // Argumenty komendy.
        char** arguments = calloc(MAX_NO_ARGUMENTS, sizeof(char*));
        for(int j = 0; j < MAX_NO_ARGUMENTS; j++)
        {
            arguments[j] = calloc(MAX_COMMAND_LENGTH, sizeof(char));
        }

        // command[prev_ptr:curr_ptr] -> "command_name", "arguments"
        parse_to_name_and_arguments(command, command_name, arguments, prev_ptr, curr_ptr);

        int fd1[2];
        int prev[2];

        if(pipe(fd1) == -1)
        {
            perror("Pipe command error!\n");
            exit(1);
        }

        pid_t child_pid = fork();

        if(child_pid == -1)
        {
            perror("Fork command error!\n");
            exit(1);
        } else if(child_pid == 0)
        {
            // CHILD PROCESS
            close(fd1[0]);
            close(prev[1]);

            if(i > 0) 
            {
                dup2(prev[0], STDIN_FILENO);
            }
            if(i < no_separated_commands - 1) 
            {
                dup2(fd1[1], STDOUT_FILENO);
            }
            execvp(command_name, arguments);
            exit(1);
        } 
        
        // Przeskakujemy na kolejna oddzielna komende.
        curr_ptr += 2;
        prev_ptr = curr_ptr;

        close(fd1[1]);
        close(prev[0]);

        prev[0] = fd1[0];
        prev[1] = fd1[1];

        for(int j = 0; j < MAX_NO_ARGUMENTS; j++)
        {
            free(arguments[j]);
        }
        free(arguments);
        free(command_name);
    }

    for(int i = 0; i < no_separated_commands; i++) 
    {
        wait(NULL);
    }
}

int main(int argc, char** argv){
    if(argc != 2){
        perror("Arguments should be: ./main file_name.txt\n");
        exit(1);
    }

    FILE* file_commands = fopen(argv[1], "r");
    if(file_commands == NULL){
        perror("Can't open the file.\n");
        exit(1);
    }

    // Pomijam argumenty tj. linie z definicjami skladnikow postaci: skladnikX = ...
    // Koncze kiedy napotkam pusta linie - ona oddziela mi argumenty od komend ( tak zakladam :) ).
    char* line = calloc(MAX_LINE_SIZE, sizeof(char));
    size_t line_buf_size = 0;
    getline(&line, &line_buf_size, file_commands);
    while(strcmp(&line[0],"\n") != 0)
    {
        getline(&line, &line_buf_size, file_commands);
    }
    free(line);

    char** non_parsed_commands = calloc(MAX_NO_COMMANDS, sizeof(char*));
    for (int i = 0; i < MAX_NO_COMMANDS; i++)
    {
        non_parsed_commands[i] = calloc(MAX_COMMAND_LENGTH, sizeof(char));
    }

    // Zapisuje stringi postaci "skladnik1 | skladnik 2 | ... | skladnikN" do tablicy.
    // Jeden taki ciag pipe'ow to jeden element tablicy.
    int i = 0;
    while(!feof(file_commands)){
        char* line = calloc(MAX_LINE_SIZE, sizeof(char));
        size_t line_buf_size = 0;
        getline(&line, &line_buf_size, file_commands);
        strcpy(non_parsed_commands[i], line);
        i++;
        free(line);
    }

    // Wykonuje kolejne komendy z tablicy.
    int command_num = 0;
    while(command_num < MAX_NO_COMMANDS && strlen(non_parsed_commands[command_num]) > 0){
        // non_parsed_command_to_exec - niesparsowana komenda do wykonania.
        // np. skladnik2 | skladnik5
        char* non_parsed_command_to_exec = calloc(MAX_COMMAND_LENGTH, sizeof(char));
        strcpy(non_parsed_command_to_exec, non_parsed_commands[command_num]);
        printf("NON_PARSED: %s",non_parsed_command_to_exec);

        // parsed_command_to_exec - sparsowana komenda, gotowa do wykonania.
        // np. ls -l | grep main
        char* parsed_command_to_exec = calloc(MAX_COMMAND_LENGTH, sizeof(char));
        parse_command(non_parsed_command_to_exec, parsed_command_to_exec, file_commands);
        printf("PARSED:     %s\n",parsed_command_to_exec);

        // Wykonanie sparsowanej komendy.
        if(strcmp(parsed_command_to_exec,"\0") != 0){
            execute_command(parsed_command_to_exec);
        }

        free(parsed_command_to_exec);
        free(non_parsed_command_to_exec);

        command_num++;

        printf("\n");
    }

    // Sprzatanie
    for(int i = 0; i < MAX_NO_COMMANDS; i++){
        free(non_parsed_commands[i]);
    }
    free(non_parsed_commands);
    fclose(file_commands);
    return 0;
}