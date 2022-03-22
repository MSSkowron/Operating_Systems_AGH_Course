CC = gcc
CCFLAGS = -Wall -g

all:
	make clean
	make child
	make main.run

clean:
	rm -f main.run
	rm -f child
	rm -f *.o

main.run: main.o
	$(CC) $(CCFLAGS) -o main.run main.o

main.o: main.c
	$(CC) $(CCFLAGS) -c main.c

child: child.o
	$(CC) $(CCFLAGS) -o child child.o

child.o: child.c
	$(CC) $(CCFLAGS) -c child.c

test:
	make all
	./main.run 7