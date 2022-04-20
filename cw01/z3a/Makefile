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

dynamic:
	make clean
	$(CC) -c -fPIC mylib.c -o mylib.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libmylib.so mylib.o
	$(CC) -fPIC -L. -o main main_dynamic.c -ldl -D DYNAMIC

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f main_dynamic

run:
	@echo -----TEST-----
	./main 6 create_table 10 wc_files large.txt large.txt large.txt large.txt large.txt large.txt large.txt large.txt large.txt large.txt print_table remove_block 1 wc_files large.txt medium.txt small.txt print_table

test:
	printf "\n" > results3a.txt
	printf "static:\n" >> results3a.txt
	printf "\n" >> results3a.txt
	make static
	make run
	printf "\n" >> results3a.txt
	printf "shared:\n" >> results3a.txt
	printf "\n" >> results3a.txt
	make shared
	make run
	printf "\n" >> results3a.txt
	printf "dynamic:\n" >> results3a.txt
	printf "\n" >> results3a.txt
	make dynamic
	make run
