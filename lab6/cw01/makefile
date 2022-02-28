CC = gcc
FLAGS = -std=c99 -Wall

all:
	make clean
	make server
	make client
server:
	$(CC) $(FLAGS) server.c queue.c -o server

client:
	$(CC) $(FLAGS) client.c queue.c -o client
clean:
	rm -rf server client
	ipcrm --all=msg