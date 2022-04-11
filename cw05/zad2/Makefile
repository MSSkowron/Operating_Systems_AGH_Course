all:
	gcc -c -Wall main.c
	gcc main.o -o main

clean:
	rm -f *.o
	rm -f main

test:
	make clean
	make all
	./main nadawca
	./main data
	./main student@poczta.pl Tytul "Sysopy to moj ulubiony przedmiot."