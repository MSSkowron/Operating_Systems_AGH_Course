CFLAGS = -Wall
CC = gcc

all:
	$(CC) -o main main.c 
	$(CC) -o calc calc.c

clean:
	rm -f main.o main calc calc.o

test: all
	@echo -e "\nTESTING\n"
	./main 6 + 5
	./main 8 x 12
	./main 144 - 137
	./main 72 / 9
	./main d a r
	./main s + t
all: main
