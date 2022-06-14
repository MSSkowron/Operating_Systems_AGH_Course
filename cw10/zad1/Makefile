CC = gcc
CFLAGS = -Wall -g

all: client server

#   	   ./client NAME    unix/inet  SOCKET_PATH

# example: ./client Mateusz unix socket   

client:
	$(CC) $(FLAGS) client.c -o client -lpthread -lm

#   	   ./server PORT   SOCKET_PATH

# example: ./server 7001 socket
server:
	$(CC) $(FLAGS) server.c -o server -lpthread -lm

clean:
	rm -f server client socket