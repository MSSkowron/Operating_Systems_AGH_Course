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
	rm -f w*

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
	printf "0.1 10:\n" > raport1.txt
	./main.run 0.1 10
	printf "\n" >> raport1.txt
	printf "0.01 100:\n" >> raport1.txt
	./main.run 0.01 100
	printf "\n" >> raport1.txt
	printf "0.000001 100:\n" >> raport1.txt
	./main.run 0.000001 100

