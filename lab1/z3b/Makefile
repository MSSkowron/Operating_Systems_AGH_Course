CC = gcc -Wall -ggdb
O = O1

static:
	make clean
	$(CC) -c mylib.c
	ar crs libmylib.a mylib.o
	$(CC) -o main main.c -L. -lmylib -$O

shared:
	make clean
	$(CC) -c -fPIC mylib.c
	$(CC) -shared -fPIC -o libmylib.so mylib.o
	$(CC) main.c -lmylib -L ./ -o main -Wl,-rpath=. -$O

dynamic:
	make clean
	$(CC) -c -fPIC mylib.c -o mylib.o
	$(CC) -Wl,-rpath=. -fPIC -shared -o libmylib.so mylib.o
	$(CC) -fPIC -L. -o main main_dynamic.c -ldl -D DYNAMIC -$O

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f main_dynamic

run:
	@echo -----TEST-----
	./main 6 create_table 10 wc_files large.txt medium.txt small.txt large.txt medium.txt small.txt large.txt medium.txt small.txt large.txt print_table remove_block 1 wc_files large.txt medium.txt small.txt print_table

test:
	printf "static O:\n" > results3b.txt
	printf "\n" >> results3b.txt
	make static O=O
	make run
	printf "\n" >> results3b.txt
	printf "static O0:\n" >> results3b.txt
	printf "\n" >> results3b.txt
	make static O=O0
	make run
	printf "\n" >> results3b.txt
	printf "static Os:\n" >> results3b.txt
	printf "\n" >> results3b.txt
	make static O=Os
	make run



