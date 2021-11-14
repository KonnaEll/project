lsh: lsh.o lsh_funcs.o
	gcc -Wall -Werror -std=c9x lsh.o lsh_funcs.o -o lsh -lm

lsh.o: lsh.c
	gcc -Wall -Werror -std=c9x -c lsh.c

lsh_funcs.o: lsh_funcs.c lsh_funcs.h
	gcc -Wall -Werror -std=c9x -c lsh_funcs.c

clean:
	rm -f lsh *.o lsh

