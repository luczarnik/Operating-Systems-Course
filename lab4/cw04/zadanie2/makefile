exec:
	gcc -Wall -std=c99 -o main main.c

clean:
	rm -rf main

run:
	make clean
	make exec
	./main SA_NOCLDWAIT
	./main SA_NOCLDSTOP