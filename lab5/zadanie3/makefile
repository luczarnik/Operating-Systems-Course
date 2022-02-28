CC =gcc -std=gnu11 -O2 -Wall -Wconversion -lm

compile:
		make clear
	        $(CC) -o main main.c
		$(CC) -o consumer consumer.c
		$(CC) -o producent producent.c
		

run:
	make clear
	make compile
	./main . wzor 3

clear:
	rm -rf main producent consumer pipe
