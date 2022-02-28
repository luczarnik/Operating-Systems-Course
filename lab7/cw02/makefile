CC=gcc 
CFLAGS=-std=gnu11 -O2 -Wall -Werror -lm

deliver:
	$(CC) $(CFLAGS) deliver.c utils.c table.c -o deliver

worker: 
	$(CC) $(CFLAGS) worker.c utils.c table.c -o worker



main:
	$(CC) $(CFLAGS) main.c utils.c table.c -o main

all:
	make clean
	make deliver
	make worker
	make main

clean:
	rm -rf deliver main worker r