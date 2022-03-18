CC = gcc
CCFLAGS = -Wall -g

test:
	make clean
	printf "Funkcje biblioteki: \n" > pomiar_zad_1.txt
	make main_lib.run
	./main_lib.run plik1.txt plik2.txt
	printf "\n" >> pomiar_zad_1.txt
	printf "Funkcje systemowe: \n" >> pomiar_zad_1.txt
	make main_sys.run
	./main_sys.run plik1.txt plik3.txt

clean:
	rm -f main_lib.run
	rm -f main_sys.run
	rm -f *.o


main_lib.run: main_lib.o functions.o
	$(CC) $(CCFLAGS) -o main_lib.run main_lib.o functions.o

main_sys.run: main_sys.o functions.o
	$(CC) $(CCFLAGS) -o main_sys.run main_sys.o functions.o

main_lib.o: main_lib.c functions.h
	$(CC) $(CCFLAGS) -c main_lib.c

main_sys.o: main_sys.c functions.h
	$(CC) $(CCFLAGS) -c main_sys.c

functions.o: functions.c functions.h
	$(CC) $(CCFLAGS) -c functions.c
