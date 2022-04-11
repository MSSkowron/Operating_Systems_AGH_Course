CC = gcc -Wall

all: producer.c consumer.c
	make clean
	$(CC) producer.c -o producer
	$(CC) consumer.c -o consumer
	touch consumer.txt

test_5_producers_1_consumer:
	make all
	mkfifo my_fifo
	@echo "5 Producers and 1 Consumer."
	@echo "40 - chars."
	@./producer my_fifo 1 producer0.txt 40 &
	@./producer my_fifo 3 producer1.txt 40 &
	@./producer my_fifo 5 producer2.txt 40 &
	@./producer my_fifo 7 producer3.txt 40 &
	@./producer my_fifo 9 producer4.txt 40 &
	@./consumer my_fifo consumer.txt 40
	cat consumer.txt
	@echo "\n1000 - chars."
	@./producer my_fifo 1 producer0.txt 1000 &
	@./producer my_fifo 3 producer1.txt 1000 &
	@./producer my_fifo 5 producer2.txt 1000 &
	@./producer my_fifo 7 producer3.txt 1000 &
	@./producer my_fifo 9 producer4.txt 1000 &
	@./consumer my_fifo consumer.txt 1000
	cat consumer.txt
	@echo "\n10000 - chars (>PIPE_BUF)."
	@./producer my_fifo 1 producer0.txt 10000 &
	@./producer my_fifo 3 producer1.txt 10000 &
	@./producer my_fifo 5 producer2.txt 10000 &
	@./producer my_fifo 7 producer3.txt 10000 &
	@./producer my_fifo 9 producer4.txt 10000 &
	@./consumer my_fifo consumer.txt 10000
	cat consumer.txt

test_1_producers_5_consumer:
	make all
	mkfifo my_fifo
	@echo "1 Producer and 5 Consumers."
	@echo "40 - chars."
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./producer my_fifo 1 producer0.txt 40
	cat consumer.txt
	@echo "\n1000 - chars."
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./producer my_fifo 1 producer0.txt 1000
	cat consumer.txt
	@echo "\n10000 - chars (>PIPE_BUF)."
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./producer my_fifo 1 producer0.txt 10000
	cat consumer.txt

test_5_producers_5_consumer:
	make all
	mkfifo my_fifo
	@echo "5 Producer and 5 Consumers."
	@echo "40 - chars."
	@./producer my_fifo 1 producer0.txt 40 &
	@./producer my_fifo 3 producer1.txt 40 &
	@./producer my_fifo 5 producer2.txt 40 &
	@./producer my_fifo 7 producer3.txt 40 &
	@./producer my_fifo 9 producer4.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40 &
	@./consumer my_fifo consumer.txt 40
	cat consumer.txt
	@echo "\n1000 - chars."
	@./producer my_fifo 1 producer0.txt 1000 &
	@./producer my_fifo 3 producer1.txt 1000 &
	@./producer my_fifo 5 producer2.txt 1000 &
	@./producer my_fifo 7 producer3.txt 1000 &
	@./producer my_fifo 9 producer4.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000 &
	@./consumer my_fifo consumer.txt 1000
	cat consumer.txt
	@echo "\n10000 - chars (>PIPE_BUF)."
	@./producer my_fifo 1 producer0.txt 10000 &
	@./producer my_fifo 3 producer1.txt 10000 &
	@./producer my_fifo 5 producer2.txt 10000 &
	@./producer my_fifo 7 producer3.txt 10000 &
	@./producer my_fifo 9 producer4.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000 &
	@./consumer my_fifo consumer.txt 10000
	cat consumer.txt

clean:
	rm -f producer
	rm -f consumer
	rm -f my_fifo
	rm -rf files
	rm -f consumer.txt