CFLAGS = -Wall
CC = gcc

zad1:
	gcc -o zad1 zad1.c

clean:
	rm -f ./zad1.o ./zad1

test: zad1
	@echo -e "\n\nTESTING\n\n"
	./zad1 plik katalog slink

all: zad1 test clean
