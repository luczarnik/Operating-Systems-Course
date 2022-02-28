static:
	make clear
	gcc -Wall -c merge.c
	ar crs libmerge.a merge.o
	gcc -Wall main.c -o main -L. -lmerge
	rm -f *.o
dynamic:
	make clear
	gcc -Wall -c -fPIC merge.c
	gcc -Wall -shared -o libmerge.so merge.o
	gcc main.c -Wall -o main -L. -lmerge -Wl,-rpath="`pwd`"
	rm -f *.o

dll:
	make clear
	gcc -Wall -c -fPIC merge.c
	gcc -Wall -shared -o libmerge.so merge.o
	gcc -Wall main.c -o main -L. -ldl -D DLL
	rm -f *.o

clear:
	rm -f *.o *.a *.so main

test:
	rm -f raport.txt
	rm -f raport2.txt
	echo "testy dla malych plikow tekstowych" >> raport2.txt
	./main create_table 13 merge_files "maleTeksty/txt1.txt:maleTeksty/txt2.txt maleTeksty/txt2.txt:maleTeksty/txt3.txt maleTeksty/txt3.txt:maleTeksty/txt4.txt maleTeksty/txt4.txt:maleTeksty/txt5.txt maleTeksty/txt5.txt:maleTeksty/txt6.txt maleTeksty/txt6.txt:maleTeksty/txt7.txt maleTeksty/txt7.txt:maleTeksty/txt8.txt maleTeksty/txt8.txt:maleTeksty/txt9.txt maleTeksty/txt9.txt:maleTeksty/txt10.txt" remove_block 1 
	cat raport.txt >> raport2.txt
	echo "testy dla duzych plikow tekstowucj" >> raport2.txt
	rm -f raport.txt
	./main create_table 15 merge_files "duzeTeksty/txt1.txt:duzeTeksty/txt2.txt duzeTeksty/txt2.txt:duzeTeksty/txt3.txt duzeTeksty/txt3.txt:duzeTeksty/txt4.txt duzeTeksty/txt4.txt:duzeTeksty/txt5.txt duzeTeksty/txt5.txt:duzeTeksty/txt6.txt" remove_block 2
	cat raport.txt >> raport2.txt
