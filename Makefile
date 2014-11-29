CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -O3 -fopenmp -lm

main: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas

gprof: joaninhas.c
	$(CC) $(CFLAGS) joaninhas.c -o joaninhas -g -pg

clean:
	@rm -rf joaninhas
