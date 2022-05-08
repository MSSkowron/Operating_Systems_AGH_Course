CC = gcc -Wall

all:
	make clean
	$(CC) cook.c -o cook -lm
	$(CC) deliverer.c -o deliverer -lm
	$(CC) main.c -o main -lm 

test:
	make all
	./main 

clean:
	rm -f cook 
	rm -f deliverer 
	rm -f main