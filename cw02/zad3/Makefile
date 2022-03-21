CC = gcc
CCFLAGS = -Wall -g

first:
	make clean
	make main_first.run
	./main_first.run katalog1

second:
	make clean
	make main_second.run
	./main_second.run katalog1

clean:
	rm -f main_first.run
	rm -f main_second.run
	rm -f *.o

main_first.run: main_first.o
	$(CC) $(CCFLAGS) -o main_first.run main_first.o

main_second.run: main_second.o
	$(CC) $(CCFLAGS) -o main_second.run main_second.o

main_first.o: main_first.c
	$(CC) $(CCFLAGS) -c main_first.c

main_second.o: main_second.c
	$(CC) $(CCFLAGS) -c main_second.c