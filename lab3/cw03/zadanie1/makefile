CC =gcc -std=gnu11 -O2 -Wall -Wconversion -lm

compile:
	$(CC) -o main main.c

run:
	make clear
	make compile
	./main 15

clear:
	rm -rf main
