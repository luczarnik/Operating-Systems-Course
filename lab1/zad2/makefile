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
	./main create_table 13 merge_files "`cat args1.txt`" remove_block 1 
	cat raport.txt >> raport2.txt
	echo "testy dla duzych plikow tekstowucj" >> raport2.txt
	rm -f raport.txt
	./main create_table 15 merge_files "`cat args2.txt`" remove_block 2
	cat raport.txt >> raport2.txt
