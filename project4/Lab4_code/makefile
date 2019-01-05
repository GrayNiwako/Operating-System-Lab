all: libmylocks.so libcounter.so liblist.so libhash.so test

test : test.c libcounter.so liblist.so libhash.so
	gcc -L. -o test test.c -lcounter -llist -lhash -lpthread -Wall -Werror
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

libhash.so : hash.o libmylocks.so liblist.so
	gcc -L. -shared -o libhash.so hash.o -lmylocks -llist

liblist.so : list.o libmylocks.so
	gcc -L. -shared -o liblist.so list.o -lmylocks

libcounter.so : counter.o libmylocks.so
	gcc -L. -shared -o libcounter.so counter.o -lmylocks

libmylocks.so : mylocks.o 
	gcc -shared -o libmylocks.so mylocks.o
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

hash.o : hash.c hash.h list.c list.h mylocks.h
	gcc -c -fpic hash.c -Wall -Werror

list.o : list.c list.h mylocks.h
	gcc -c -fpic list.c -Wall -Werror

counter.o : counter.c counter.h mylocks.h
	gcc -c -fpic counter.c -Wall -Werror

mylocks.o : mylocks.c mylocks.h xchg.c sys_futex.c
	gcc -c -fpic mylocks.c -Wall -Werror

clean : 
	 rm -f libcounter.so liblist.so libhash.so libmylocks.so mylocks.o counter.o list.o hash.o test

