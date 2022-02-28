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
                                   
