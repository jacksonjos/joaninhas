CC=gcc
FLAGS=-Wall -std=c11 -pedantic -O3

main: joaninhas.c
	$(CC) joaninhas.c -o exe

flags: joaninhas.c
	$(CC) $(FLAGS) joaninhas.c -o exe

clean:
	rm -rf exe
