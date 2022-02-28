O="O2"

static:
	make clear
	gcc -Wall -c -$O merge.c
	ar crs libmerge.a merge.o
	gcc -Wall main.c -o main -$O -L. -lmerge
	rm -f *.o
dynamic:
	make clear
	gcc -Wall -$O -c -fPIC merge.c
	gcc -Wall -$O -shared -o libmerge.so merge.o
	gcc main.c -Wall -o  main -$O -L. -lmerge -Wl,-rpath="`pwd`"
	rm -f *.o

dll:
	make clear
	gcc -Wall -c -fPIC merge.c -$O
	gcc -Wall -shared -o libmerge.so merge.o -$O
	gcc -Wall main.c -o main -L. -ldl -D DLL -$O
	rm -f *.o 

clear:
	rm -f *.o *.a *.so main

test:
	rm -f raport.txt
	echo "testy dla malych plikow tekstowych" >> results3b.txt
	./main create_table 13 merge_files "`cat args1.txt`" remove_block 1 
	cat raport.txt >> results3b.txt
	echo "testy dla duzych plikow tekstowucj" >> results3b.txt
	rm -f raport.txt
	./main create_table 15 merge_files "`cat args2.txt`" remove_block 2
	cat raport.txt >> results3b.txt

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
	echo "	linkowanie statyczne">> results3b.txt
	make statictest
	echo "	linkowanie wspoldzielone" >> results3b.txt
	make dynamictest
	echo "	linkowanie dynamiczne " >> results3b.txt
	make dlltest

optimizedTests:
	rm -f results3b.txt
	echo "     -O optimalization">>results3b.txt
	O="-O"
	make runtests
	echo "     -O1 optimalization">>results3b.txt
	O="-O1"
	make runtests
	echo "     -O2 optimalization">>results3b.txt
	O="-O2"
	make runtests
	echo "     -O3 optimalization">>results3b.txt
	O="-O3"
	make runtests
	echo "     -O0 optimalization">>results3b.txt
	O="-O0"
	make runtests
	echo "     -Os optimalization">>results3b.txt
	O="-Os"
	make runtests
