all: defrag check

defrag: defrag.c defrag.h
	gcc -Wall -Werror -o defrag defrag.c defrag.h

check: check.c defrag.h
	gcc -Wall -Werror -o check check.c defrag.h

clean:
	rm defrag check
