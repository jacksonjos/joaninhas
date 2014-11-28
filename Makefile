CC=gcc
FLAGS=-Wall -ansi -pedantic -O2

main: joaninhas.c
	$(CC) joaninhas.c -o exe

flags: joaninhas.c
	$(CC) $(FLAGS) joaninhas.c -o exe

clean:
	rm -rf exe
