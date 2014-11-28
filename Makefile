CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -O3 -fopenmp

main: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas -lm

clean:
	rm -rf joaninhas
