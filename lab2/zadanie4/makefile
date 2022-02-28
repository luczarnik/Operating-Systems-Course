system:
	gcc -std=gnu11 -O2 -Wall -o main main.c -lm
	./main file1.txt file2.txt equal somepattern
language:
	gcc -std=gnu11 -O2 -Wall -o main2 main2.c -lm
	./main2 file1.txt file2.txt equal somepattern
main:
	make clear
	echo "biblioteki systemowe" >> pomiar_zad_4.txt
	make system
	echo "biblioteki jezyka" >> pomiar_zad_4.txt
	make language
clear:
	rm -f main pomiar_zad_4.txt main2 file2.txt
