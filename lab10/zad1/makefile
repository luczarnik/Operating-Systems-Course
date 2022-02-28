CC = gcc
CFLAGS = -Wall -std=c11 -g -pthread

all: 
	make clean
	make server
	make client

client:
	$(CC) $(CFLAGS) -o client client.c

server:
	$(CC) $(CFLAGS) -o server server.c


clean:
	rm -f server client socket