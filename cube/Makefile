cube: cube.o cube_funcs.o lsh_funcs.o
	gcc -Wall -Werror -std=c9x cube.o cube_funcs.o lsh_funcs.o -o cube -lm

cube.o: cube.c
	gcc -Wall -Werror -std=c9x -c cube.c

cube_funcs.o: cube_funcs.c cube_funcs.h
	gcc -Wall -Werror -std=c9x -c cube_funcs.c

lsh_funcs.o: lsh_funcs.c lsh_funcs.h
	gcc -Wall -Werror -std=c9x -c lsh_funcs.c

clean:
	rm -f cube*.o lsh*.o cube

