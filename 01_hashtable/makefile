all:program

program:hash.o libhash.so
	LD_LIBRARY_PATH=/Users/lokotochek/Documents/acos-15/13_laktionov/01_hashtable:$LD_LIBRARY_PATH
		gcc -L/Users/lokotochek/Documents/acos-15/13_laktionov/01_hashtable tests.c -o program -lhash

libhash.so: hash.o
	gcc -shared -o libhash.so hash.o

tests.o:
	gcc -c -Wvisibility tests.c

tests.c:

hash.o: hash.c
	gcc -fPIC -c -Wvisibility hash.c

hash.c:
        
clean:
	rm *.o