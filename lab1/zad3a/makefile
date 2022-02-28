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
	echo "testy dla malych plikow tekstowych" >> results3a.txt
	./main create_table 13 merge_files "`cat args1.txt`" remove_block 1 
	cat raport.txt >> results3a.txt
	echo "testy dla duzych plikow tekstowucj" >> results3a.txt
	rm -f raport.txt
	./main create_table 15 merge_files "`cat args2.txt`" remove_block 2
	cat raport.txt >> results3a.txt

statictest:
	make static
	make test
dynamictest:
	make dynamic
	make test
dlltest:
	make dll
	make test
	
runtests:
	rm -f results3a.txt
	echo "linkowanie statyczne">> results3a.txt
	make statictest
	echo "linkowanie wspoldzielone" >> results3a.txt
	make dynamictest
	echo "linkowanie dynamiczne " >> results3a.txt
	make dlltest
