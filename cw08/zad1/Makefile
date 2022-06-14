all: main

main: clean
	gcc -c -Wall main.c
	gcc main.o -o main -lpthread

tests: main test-blocks test-numbers

test-blocks:
	echo "[BLOCK TEST]" >> Times.txt
	./main 1 block ./mona_lisa.ascii.pgm ./results/block-mona_lisa_1.ascii.pgm >> Times.txt 
	echo " ">> Times.txt 
	echo "[BLOCK TEST]" >> Times.txt
	./main 2 block ./mona_lisa.ascii.pgm ./results/block-mona_lisa_2.ascii.pgm >> Times.txt 
	echo " ">> Times.txt 
	echo "[BLOCK TEST]" >> Times.txt
	./main 4 block ./mona_lisa.ascii.pgm ./results/block-mona_lisa_4.ascii.pgm >> Times.txt 
	echo " ">> Times.txt 
	echo "[BLOCK TEST]" >> Times.txt
	./main 8 block ./mona_lisa.ascii.pgm ./results/block-mona_lisa_8.ascii.pgm >> Times.txt 
	echo " ">> Times.txt 
	echo "[BLOCK TEST]" >> Times.txt
	./main 16 block ./mona_lisa.ascii.pgm ./results/block-mona_lisa_16.ascii.pgm >> Times.txt 
	echo " ">> Times.txt 

test-numbers:
	echo "[NUMBERS TEST]" >> Times.txt
	./main 1 numbers ./mona_lisa.ascii.pgm ./results/numbers-mona_lisa_1.ascii.pgm >> Times.txt;
	echo " ">> Times.txt 
	echo "[NUMBERS TEST]" >> Times.txt
	./main 2 numbers ./mona_lisa.ascii.pgm ./results/numbers-mona_lisa_2.ascii.pgm >> Times.txt;
	echo " ">> Times.txt 
	echo "[NUMBERS TEST]" >> Times.txt
	./main 4 numbers ./mona_lisa.ascii.pgm ./results/numbers-mona_lisa_4.ascii.pgm >> Times.txt;
	echo " ">> Times.txt 
	echo "[NUMBERS TEST]" >> Times.txt
	./main 8 numbers ./mona_lisa.ascii.pgm ./results/numbers-mona_lisa_8.ascii.pgm >> Times.txt;
	echo " ">> Times.txt 
	echo "[NUMBERS TEST]" >> Times.txt
	./main 16 numbers ./mona_lisa.ascii.pgm ./results/numbers-mona_lisa_16.ascii.pgm >> Times.txt;
	echo " ">> Times.txt 

clean:
	rm -f Times.txt
	rm -f *.o
	rm -f main
