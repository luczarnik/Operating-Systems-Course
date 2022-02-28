CC =gcc -std=gnu11 -O2 -Wall -Wconversion -Werror -lm


fork:
    echo Polecenie fork:
	make clean
	$(CC) -o main main.c
	echo ignorowanie sygalu:
	./main ignore fork
	echo przejmowanie sygnalu:
	./main handler fork
	echo maskowanie sygnalu:
	./main mask fork
	echo zwlekanie z odebraniem sygnalu:
	./main pending fork

exec:
	make clean
	echo Polecenie exec:
	$(CC) -o main main.c
	$(CC) -o "exec" exec.c
	echo ignorowanie sygalu:
	./main ignore "exec"
	echo przejmowanie sygnalu:
	./main "handler" 'exec'
	echo maskowanie sygnalu:
	./main mask "exec"
	echo zwlekanie z odebraniem sygnalu:
	./main pending "exec"


clean:
	rm -rf main exec
