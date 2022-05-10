 CC = gcc
 flags = -Wall -lpthread -lrt

all:
	make clean
	$(CC) cook.c -o cook $(flags)
	$(CC) deliverer.c -o deliverer $(flags)
	$(CC) main.c -o main $(flags)

test:
	make all
	./main 

clean:
	rm -f cook 
	rm -f deliverer 
	rm -f main