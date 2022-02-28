system:
	gcc -std=gnu11 -O2 -Wall -o main main.c -lm
	./main a file1.txt
language:
	gcc -std=gnu11 -O2 -Wall -o main2 main2.c -lm
	./main2 a file1.txt
main:
	make clear
	echo "biblioteki systemowe" >> pomiar_zad_2.txt
	make system
	echo "biblioteki jezyka" >> pomiar_zad_2.txt
	make language
clear:
	rm -f main pomiar_zad_2.txt main2
