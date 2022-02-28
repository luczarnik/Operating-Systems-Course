CC= gcc
CFLAGS=-std=gnu11 -O2 -Wall
SOURCE=main.c pbma/pbma_io.c utilities.c
LIBRARIES = -lm -pthread

all:
	make clean
	$(CC) $(CFLAGS) $(SOURCE) -o main $(LIBRARIES)

clean:
	rm -rf main images/output.pgm

run:
	./main 15 block images/input.pgm images/output.pgm
