CC = gcc
FLAGS = -Wall

compile:
	$(CC) $(FLAGS) main.c -o main
	$(CC) $(FLAGS) exec.c -o exec

clean:
	rm -f main
	rm -f exec

testFork:
	make clean
	make compile
	./main ignore fork
	./main pending fork
	./main mask fork
	./main handler fork

testExec:
	make clean
	make compile
	./main ignore exec
	./main pending exec
	./main mask exec