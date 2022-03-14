CC = gcc -Wall -ggdb

all: clean static shared

static:
	$(CC) -c mylib.c
	ar crs libmylib.a mylib.o

shared:
	$(CC) -c -fPIC mylib.c
	$(CC) -shared -fPIC -o libmylib.so mylib.o

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f *.txt
