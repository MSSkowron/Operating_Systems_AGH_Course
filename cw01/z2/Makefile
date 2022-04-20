CC = gcc -Wall -ggdb

static:
	make clean
	$(CC) -c mylib.c
	ar crs libmylib.a mylib.o
	$(CC) -o main main.c -L. -lmylib

shared:
	make clean
	$(CC) -c -fPIC mylib.c
	$(CC) -shared -fPIC -o libmylib.so mylib.o
	$(CC) main.c -lmylib -L ./ -o main -Wl,-rpath=.

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main

run:
	@echo -----TEST-----
	make static
	./main 6 create_table 2 wc_files large.txt medium.txt small.txt print_table remove_block 1 wc_files large.txt medium.txt small.txt print_table
