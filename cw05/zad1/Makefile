all:
	gcc -c -Wall main.c
	gcc main.o -o main

clean:
	rm -f *.o
	rm -f main

test:
	make clean
	make all
	./main commands.txt
	