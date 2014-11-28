CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -O3 -fopenmp

main: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o exe

clean:
	rm -rf exe
